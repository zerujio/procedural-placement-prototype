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

    // 1 Load shaders
    // 1.1 Compute shader for terrain generation
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

    // 2 Allocate/initialize GPU memory
    // 2.1 Calculate index/vertex count
    const glm::ivec3 num_work_groups {8, 8, 1};
    glm::ivec3 work_group_size;
    glGetProgramiv(compute_program->id(), GL_COMPUTE_WORK_GROUP_SIZE, glm::value_ptr(work_group_size));
    const glm::ivec3 num_compute_threads = num_work_groups * work_group_size;

    GLsizei vertex_count = num_compute_threads.x * num_compute_threads.y;
    GLsizei index_count = (num_compute_threads.x - 1) * (num_compute_threads.y - 1) * 6;

    // 2.2 Create buffers
    GL::ObjectManager<GL::Buffer> vertex_buffer;
    vertex_buffer->bind(GL::Buffer::Target::Array);
    GL::Buffer::setData(GL::Buffer::Target::Array,
                        vertex_count * (2 * sizeof(glm::vec4) + sizeof(glm::vec2)),
                        nullptr,
                        GL::Buffer::Usage::StaticDraw);

    GL::ObjectManager<GL::Buffer> index_buffer;
    index_buffer->bind(GL::Buffer::Target::ElementArray);
    GL::Buffer::setData(GL::Buffer::Target::ElementArray,
                        index_count * sizeof(unsigned int),
                        nullptr,
                        GL::Buffer::Usage::StaticDraw);

    // 3 Generate Terrain
    struct GPUMemoryRange {
        GLintptr offset;
        GLsizeiptr size;
    };

    GPUMemoryRange index_mem {0, index_count * static_cast<GLsizei>(sizeof(unsigned int))};

    // vec3's are padded to sizeof(vec4) in SSB0's
    GPUMemoryRange position_mem {0, vertex_count * static_cast<GLsizei>(sizeof(glm::vec4))};
    GPUMemoryRange normal_mem {position_mem.size, vertex_count * static_cast<GLsizei>(sizeof(glm::vec4))};
    GPUMemoryRange tex_coord_mem {normal_mem.offset + normal_mem.size, vertex_count * static_cast<GLsizei>(sizeof(glm::vec2))};

    // 3.1 Bind SSBO
    compute_program->useProgram();

    vertex_buffer->bind(GL::Buffer::Target::ShaderStorage);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, /*biding=*/0, vertex_buffer->id(),
                      position_mem.offset, position_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, vertex_buffer->id(), normal_mem.offset, normal_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, vertex_buffer->id(), tex_coord_mem.offset, tex_coord_mem.size);

    index_buffer->bind(GL::Buffer::Target::ShaderStorage);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, index_buffer->id(), index_mem.offset, index_mem.size);

    // 3.2 Bind heightmap texture
    auto heightmap_tex_data = loadTexture("heightmap.png");
    GL::ObjectManager<GL::Texture> heightmap_texture {heightmap_tex_data.gl_texture};
    GL::Texture::setActive(0);
    heightmap_texture->bind(GL::Texture::Target::Tex2D);

    // 3.3 Execute compute shader
    glDispatchCompute(num_work_groups.x, num_work_groups.y, num_work_groups.z);

    // 4. Set up VAOs for rendering

    constexpr unsigned int a_position_location {0};
    constexpr unsigned int a_normal_location {1};
    constexpr unsigned int a_texCoord_location {2};

    // 4.1 Terrain
    GL::ObjectManager<GL::VertexArray> terrain_vao;
    terrain_vao->bind();
    vertex_buffer->bind(GL::Buffer::Target::Array);
    index_buffer->bind(GL::Buffer::Target::ElementArray);

    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                          reinterpret_cast<void*>(position_mem.offset));
    glEnableVertexAttribArray(a_position_location);

    glVertexAttribPointer(a_normal_location, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                          reinterpret_cast<void*>(normal_mem.offset));
    glEnableVertexAttribArray(a_normal_location);

    glVertexAttribPointer(a_texCoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                          reinterpret_cast<void*>(tex_coord_mem.offset));
    glEnableVertexAttribArray(a_texCoord_location);

    // 5. Calculate transforms, set uniforms

    constexpr int u_model_loc = 0;
    constexpr int u_view_loc = 1;
    constexpr int u_proj_loc = 2;
    constexpr int u_view_pos_loc = 7;

    auto terrain_transform = glm::scale(glm::mat4(1.0f), {10, 0.52, 7.62});
    terrain_transform = glm::translate(terrain_transform, {-.5, 0, -.5});

    float camera_angle = 3.14f;
    glm::vec3 camera_position {0.0f, 2.5f, -5.0f};
    glm::mat4 view_mtx = glm::lookAt(camera_position, glm::vec3(0.0f), {0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mtx = glm::perspectiveFov(70.0f, 1024.0f, 768.0f, 0.01f, 100.0f);

    lighting_program->useProgram();
    glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, glm::value_ptr(terrain_transform));
    glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, glm::value_ptr(proj_mtx));
    glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, glm::value_ptr(view_mtx));
    glUniform3fv(u_view_pos_loc, 1, glm::value_ptr(camera_position));

    point_program->useProgram();
    glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, glm::value_ptr(terrain_transform));
    glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, glm::value_ptr(view_mtx));
    glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, glm::value_ptr(proj_mtx));

    // 6. Render
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // 7. Debug?
    //auto vertices = reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
    //glUnmapBuffer(GL_ARRAY_BUFFER);

    auto indices = reinterpret_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY));
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(window.ptr())) {
        double now = glfwGetTime();
        double delta = now - last;
        last = now;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const int direction = glfwGetKey(window.ptr(), GLFW_KEY_D) - glfwGetKey(window.ptr(), GLFW_KEY_A);
        if (direction) {
            constexpr float camera_angular_speed {1.57f};
            constexpr float camera_radius {5.0f};
            camera_angle += camera_angular_speed * delta * direction;
            camera_position.x = std::sin(camera_angle) * camera_radius;
            camera_position.z = std::cos(camera_angle) * camera_radius;
            view_mtx = glm::lookAt(camera_position, glm::vec3(0.0f), {0.0f, 1.0f, 0.0f});
        }

        terrain_vao->bind();

        lighting_program->useProgram();

        if (direction) {
            glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, glm::value_ptr(view_mtx));
            glUniform3fv(u_view_pos_loc, 1, glm::value_ptr(camera_position));
        }

        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT,
                       reinterpret_cast<const void *>(index_mem.offset));

        point_program->useProgram();

        if (direction)
            glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, glm::value_ptr(view_mtx));

        glDrawArrays(GL_POINTS, 0, vertex_count);

        glfwSwapBuffers(window.ptr());
        glfwPollEvents();
    }

    return 0;
}