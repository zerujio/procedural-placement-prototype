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

GL::Texture loadTexture(const std::string &path) {
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

    return texture;
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

    // START: generate point cloud
    GL::ObjectManager<GL::ShaderProgram> compute_program;
    GL::ObjectManager<GL::Shader> compute_shader {loadShader("shaders/heightmap.comp", GL::ShaderType::Compute)};
    compute_program.handle().attachShader(compute_shader.handle());
    compute_program.handle().linkProgram();
    compute_program.handle().useProgram();

    glm::ivec3 num_work_groups {8, 8, 1};
    glm::ivec3 work_group_size;
    glGetProgramiv(compute_program.handle().id(), GL_COMPUTE_WORK_GROUP_SIZE, glm::value_ptr(work_group_size));
    glm::ivec3 point_cloud_size {num_work_groups * work_group_size};
    auto point_cloud_stride = sizeof(glm::vec4);

    GL::ObjectManager<GL::Buffer> point_cloud_buffer;
    point_cloud_buffer.handle().bind(GL::Buffer::Target::ShaderStorage);
    GL::Buffer::setData(GL::Buffer::Target::ShaderStorage,
                        point_cloud_size.x * point_cloud_size.y * point_cloud_size.z * point_cloud_stride,
                        nullptr,
                        GL::Buffer::Usage::StaticDraw);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, /*binding=*/0, point_cloud_buffer.handle().id());

    GL::ObjectManager<GL::Texture> texture {loadTexture("heightmap.png")};
    texture.handle().bind(GL::Texture::Target::Tex2D);

    glDispatchCompute(num_work_groups.x, num_work_groups.y, num_work_groups.z);

    auto point_cloud_transform = glm::scale(glm::mat4(1.0f), {10, 0.52, 7.62});
    point_cloud_transform = glm::translate(point_cloud_transform, {-.5, 0, -.5});

    // END : generate point cloud

    // START : point shader

    GL::ObjectManager<GL::ShaderProgram> point_program;
    {
        GL::ObjectManager vertex {loadShader("shaders/point.vert", GL::ShaderType::Vertex)};
        GL::ObjectManager fragment {loadShader("shaders/point.frag", GL::ShaderType::Fragment)};

        point_program.handle().attachShader(vertex.handle());
        point_program.handle().attachShader(fragment.handle());
        point_program.handle().linkProgram();
    }

    GL::ObjectManager<GL::VertexArray> point_cloud_vao_manager;
    auto point_cloud_vao = point_cloud_vao_manager.handle();
    point_cloud_vao.bind();
    point_cloud_buffer.handle().bind(GL::Buffer::Target::Array);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, point_cloud_stride, (void*) nullptr);
    glEnableVertexAttribArray(0);

    // END : point shader

    GL::ObjectManager<GL::ShaderProgram> shader_program {};
    {
        GL::ObjectManager<GL::Shader> vertex_shader {loadShader("shaders/lighting.vert", GL::ShaderType::Vertex)};
        shader_program.handle().attachShader(vertex_shader.handle());

        GL::ObjectManager<GL::Shader> fragment_shader {loadShader("shaders/lighting.frag", GL::ShaderType::Fragment)};
        shader_program.handle().attachShader(fragment_shader.handle());

        shader_program.handle().linkProgram();
    }


    GL::ObjectManager<GL::VertexArray> vao {};
    vao.handle().bind();

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    const Vertex vertices[] {
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

    GL::ObjectManager<GL::Buffer> vbo_manager {};
    auto vbo = vbo_manager.handle();
    vbo.bind(GL::Buffer::Target::Array);
    GL::Buffer::setData(GL::Buffer::Target::Array, sizeof(vertices), vertices, GL::Buffer::Usage::StaticDraw);

    const unsigned int indices[] {
        1, 3, 0,    0, 3, 2,
        6, 5, 4,    5, 6, 7,
        9, 10, 8,   10, 9, 11,
        14, 13, 12, 13, 14, 15,
        19, 16, 18, 16, 19, 17,
        22, 21, 23, 21, 22, 20
    };

    GL::ObjectManager<GL::Buffer> ebo_manager {};
    auto ebo = ebo_manager.handle();
    ebo.bind(GL::Buffer::Target::ElementArray);
    GL::Buffer::setData(GL::Buffer::Target::ElementArray, sizeof(indices), indices, GL::Buffer::Usage::StaticDraw);

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

    glm::vec3 camera_position {0.0f, 2.5f, -5.0f};
    glm::mat4 view_mtx = glm::lookAt(camera_position, glm::vec3(0.0f), {0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mtx = glm::perspectiveFov(70.0f, 1024.0f, 768.0f, 0.01f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    shader_program.handle().useProgram();
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view_mtx));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(proj_mtx));
    glUniform3fv(7, 1, glm::value_ptr(camera_position));

    point_program.handle().useProgram();
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view_mtx));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(proj_mtx));

    while (!glfwWindowShouldClose(window.ptr())) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        constexpr float angular_speed = 1.0f;
        float angle = angular_speed * static_cast<float>(glfwGetTime());
        glm::mat4 model_mtx = glm::rotate(glm::mat4(1.0f), angle, {0.0f, 1.0f, 0.0f});
        model_mtx = glm::rotate(model_mtx, angle, {0.0f, 0.0f, 1.0f});

        vao.handle().bind();
        shader_program.handle().useProgram();
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model_mtx));
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);

        point_cloud_vao.bind();
        point_program.handle().useProgram();
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(point_cloud_transform));
        glDrawArrays(GL_POINTS, 0, point_cloud_size.x * point_cloud_size.y * point_cloud_size.z);

        glfwSwapBuffers(window.ptr());
        glfwPollEvents();
    }

    return 0;
}