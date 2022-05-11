#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glfw_utils/window.hpp"
#include "glfw_utils/init.hpp"

#include "gl_utils/gl.hpp"

#include "utils/image.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

void glfwCallback(int error_code, const char *description) {
    std::cerr << "[GLFW ERROR " << error_code << "] " << description << std::endl;
}

GL::Shader loadShader(const std::string &path, GL::ShaderType shader_type) {
    std::fstream file {path};
    auto shader = GL::Shader::create(shader_type);
    shader.setSource((std::stringstream() << file.rdbuf()).str());
    shader.compileShader();

    return shader;
}

struct TextureData {
    GL::Texture gl_texture;
    glm::ivec2 dimensions;
};

TextureData loadTexture(const std::string &path) {
    Image image(path);

    using IFormat = GL::Texture::InternalFormat;
    using Format = GL::Texture::Format;
    using Type = GL::Texture::Type;


    const std::pair<IFormat, Format> formats[] {
            {IFormat::R8, Format::Red},
            {IFormat::RG8, Format::RG},
            {IFormat::RGB8, Format::RGB},
            {IFormat::RGBA8, Format::RGBA}
    };

    auto iformat = formats[image.channels() - 1].first;
    auto format = formats[image.channels() - 1].second;

    auto texture = GL::Texture::create();
    auto target = GL::Texture::Target::Tex2D;
    texture.bind(target);
    GL::Texture::texImage2D(target, 0,
                            iformat,
                            image.dimensions().x, image.dimensions().y,
                            format,
                            Type::UByte,
                            image.data());
    GL::Texture::generateMipmap(target);

    return {texture, image.dimensions()};
}

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

constexpr Vertex cube_vertices[] {
        {{ .5, .5,-.5}, { .0, .0,-1.}, {1., 0.}}, // 0
        {{ .5,-.5,-.5}, { .0, .0,-1.}, {1., 1.}},
        {{-.5, .5,-.5}, { .0, .0,-1.}, {0., 0.}},
        {{-.5,-.5,-.5}, { .0, .0,-1.}, {0., 1.}},

        {{ .5, .5, .5}, { .0, .0, 1.}, {1., 0.}}, // 4
        {{ .5,-.5, .5}, { .0, .0, 1.}, {1., 1.}},
        {{-.5, .5, .5}, { .0, .0, 1.}, {0., 0.}},
        {{-.5,-.5, .5}, { .0, .0, 1.}, {0., 1.}},

        {{ .5, .5, .5}, { 0., 1., 0.}, {1., 0.}}, // 8
        {{ .5, .5,-.5}, { 0., 1., 0.}, {1., 1.}},
        {{-.5, .5, .5}, { 0., 1., 0.}, {0., 0.}},
        {{-.5, .5,-.5}, { 0., 1., 0.}, {0., 1.}},

        {{ .5,-.5, .5}, { 0.,-1., 0.}, {1., 0.}}, // 12
        {{ .5,-.5,-.5}, { 0.,-1., 0.}, {1., 1.}},
        {{-.5,-.5, .5}, { 0.,-1., 0.}, {0., 0.}},
        {{-.5,-.5,-.5}, { 0.,-1., 0.}, {0., 1.}},

        {{ .5, .5, .5}, { 1., 0., 0.}, {1., 0.}}, // 16
        {{ .5, .5,-.5}, { 1., 0., 0.}, {1., 1.}},
        {{ .5,-.5, .5}, { 1., 0., 0.}, {0., 0.}},
        {{ .5,-.5,-.5}, { 1., 0., 0.}, {0., 1.}},

        {{-.5, .5, .5}, {-1., 0., 0.}, {1., 0.}}, // 20
        {{-.5, .5,-.5}, {-1., 0., 0.}, {1., 1.}},
        {{-.5,-.5, .5}, {-1., 0., 0.}, {0., 0.}},
        {{-.5,-.5,-.5}, {-1., 0., 0.}, {0., 1.}},
};

constexpr unsigned int cube_indices[] {
        1, 3, 0,    0, 3, 2,
        6, 5, 4,    5, 6, 7,
        9, 10, 8,   10, 9, 11,
        14, 13, 12, 13, 14, 15,
        19, 16, 18, 16, 19, 17,
        22, 21, 23, 21, 22, 20
};

int main() {

    glfwSetErrorCallback(glfwCallback);

    GLFW::InitSentry _glfw_init_sentry;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    GLFW::Window window{1024, 768, "Hello world!"};

    glfwMakeContextCurrent(window.ptr());

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "OpenGL initialization failed" << std::endl;
        return -1;
    }

    GL::enableDebugMessageCallback();

    // 1. Load shaders

    // 1.1 Compute shader for point cloud generation
    GL::ObjectManager<GL::ShaderProgram> compute_program;
    {
        GL::ObjectManager<GL::Shader> compute_shader{loadShader("shaders/heightmap.comp", GL::ShaderType::Compute)};
        compute_program->attachShader(compute_shader.handle());
        compute_program->linkProgram();
    }

    // 1.2 Simple shader for point cloud rendering
    GL::ObjectManager<GL::ShaderProgram> point_program;
    {
        GL::ObjectManager vertex{loadShader("shaders/point.vert", GL::ShaderType::Vertex)};
        GL::ObjectManager fragment{loadShader("shaders/point.frag", GL::ShaderType::Fragment)};

        point_program->attachShader(vertex.handle());
        point_program->attachShader(fragment.handle());
        point_program->linkProgram();
    }

    // 1.3 Shader with lighting
    GL::ObjectManager<GL::ShaderProgram> lighting_program;
    {
        GL::ObjectManager vertex_shader{loadShader("shaders/lighting.vert", GL::ShaderType::Vertex)};
        GL::ObjectManager fragment_shader{loadShader("shaders/lighting.frag", GL::ShaderType::Fragment)};
        lighting_program->attachShader(vertex_shader.handle());
        lighting_program->attachShader(fragment_shader.handle());
        lighting_program->linkProgram();
    }

    // 2. Allocate/initialize GPU memory
    struct GPUMemoryRange {
        GLsizei size;
        GLsizei offset;
    };

    // 2.1 Vertex buffer
    GL::ObjectManager<GL::Buffer> vertex_buffer;
    vertex_buffer->bind(GL::Buffer::Target::Array);

    // 2.1.1 Calculate size of point cloud
    const glm::ivec3 num_work_groups{8, 8, 1};
    glm::ivec3 work_group_size;
    glGetProgramiv(compute_program->id(), GL_COMPUTE_WORK_GROUP_SIZE, glm::value_ptr(work_group_size));
    const glm::ivec3 point_cloud_dim{num_work_groups * work_group_size};
    const int point_count = point_cloud_dim.x * point_cloud_dim.y * point_cloud_dim.z;

    // 2.1.2 Allocate vertex buffer
    // vec3 is padded to sizeof(vec4) when in a SSBO
    const GPUMemoryRange cube_mem{sizeof(cube_vertices), 0};
    const GPUMemoryRange point_mem{static_cast<int>(sizeof(glm::vec4) * point_count), cube_mem.size};
    GL::Buffer::setData(GL::Buffer::Target::Array,
                        point_mem.size + cube_mem.size,
                        nullptr,
                        GL::Buffer::Usage::StaticDraw);
    glBufferSubData(GL_ARRAY_BUFFER, cube_mem.offset, cube_mem.size, cube_vertices);

    // 2.2 Index buffer
    GL::ObjectManager<GL::Buffer> index_buffer;
    index_buffer->bind(GL::Buffer::Target::ElementArray);
    GL::Buffer::setData(GL::Buffer::Target::ElementArray,
                        sizeof(cube_vertices),
                        cube_vertices,
                        GL::Buffer::Usage::StaticDraw);


    // 3. Generate point cloud
    compute_program->useProgram();
    vertex_buffer->bind(GL::Buffer::Target::ShaderStorage);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, /*biding=*/0, vertex_buffer->id(), point_mem.offset, point_mem.size);

    auto heightmap_tex_data = loadTexture("heightmap.png");
    GL::ObjectManager<GL::Texture> heightmap_texture {heightmap_tex_data.gl_texture};
    GL::Texture::setActive(0);
    heightmap_texture->bind(GL::Texture::Target::Tex2D);

    glDispatchCompute(num_work_groups.x, num_work_groups.y, num_work_groups.z);

    // 4. Set up VAOs for rendering
    vertex_buffer->bind(GL::Buffer::Target::Array);

    // 4.1 Point cloud
    GL::ObjectManager<GL::VertexArray> point_cloud_vao;
    point_cloud_vao->bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*) point_mem.offset);
    glEnableVertexAttribArray(0);

    // 4.2 Cube mesh
    GL::ObjectManager<GL::VertexArray> cube_vao;
    cube_vao->bind();
    index_buffer->bind(GL::Buffer::Target::ElementArray);

    constexpr unsigned int a_position_location {0};
    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(a_position_location);

    constexpr unsigned int a_normal_location {1};
    glVertexAttribPointer(a_normal_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(a_normal_location);

    constexpr unsigned int a_texCoord_location {2};
    glVertexAttribPointer(a_texCoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    glEnableVertexAttribArray(a_texCoord_location);

    // 5. Set static uniforms for rendering
    auto point_cloud_transform = glm::scale(glm::mat4(1.0f), {10, 0.52, 7.62});
    point_cloud_transform = glm::translate(point_cloud_transform, {-.5, 0, -.5});

    glm::vec3 camera_position {0.0f, 2.5f, -5.0f};
    glm::mat4 view_mtx = glm::lookAt(camera_position, glm::vec3(0.0f), {0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mtx = glm::perspectiveFov(70.0f, 1024.0f, 768.0f, 0.01f, 100.0f);

    lighting_program->useProgram();
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view_mtx));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(proj_mtx));
    glUniform3fv(7, 1, glm::value_ptr(camera_position));

    point_program->useProgram();
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view_mtx));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(proj_mtx));

    // 6. Render
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // debug
    //auto data = static_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
    //glUnmapBuffer(GL_ARRAY_BUFFER);

    while (!glfwWindowShouldClose(window.ptr())) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        constexpr float angular_speed = 1.0f;
        float angle = angular_speed * static_cast<float>(glfwGetTime());
        glm::mat4 model_mtx = glm::rotate(glm::mat4(1.0f), angle, {0.0f, 1.0f, 0.0f});
        model_mtx = glm::rotate(model_mtx, angle, {0.0f, 0.0f, 1.0f});

        cube_vao->bind();
        lighting_program->useProgram();
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model_mtx));
        glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);

        point_cloud_vao->bind();
        point_program->useProgram();
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(point_cloud_transform));
        glDrawArrays(GL_POINTS, 0, point_count);

        glfwSwapBuffers(window.ptr());
        glfwPollEvents();
    }

    return 0;
}