#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glfw_utils/window.hpp"
#include "glfw_utils/init.hpp"

#include "gl_utils/gl.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

void glfwCallback(int error_code, const char *description) {
    std::cerr << "[GLFW ERROR " << error_code << "] " << description << std::endl;
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

    GL::ObjectManager<GL::ShaderProgram> shader_program {};
    {
        GL::ObjectManager<GL::Shader> vshader_mngr {GL::ShaderType::Vertex};
        {
            std::fstream file {"shaders/simple.vert"};
            vshader_mngr.handle().setSource((std::stringstream() << file.rdbuf()).str());
        }
        vshader_mngr.handle().compileShader();
        shader_program.handle().attachShader(vshader_mngr.handle());

        GL::ObjectManager<GL::Shader> fshader_mngr {GL::ShaderType::Fragment};
        {
            std::fstream file {"shaders/simple.frag"};
            fshader_mngr.handle().setSource((std::stringstream() << file.rdbuf()).str());
        }
        fshader_mngr.handle().compileShader();
        shader_program.handle().attachShader(fshader_mngr.handle());

        shader_program.handle().linkProgram();
    }


    GL::ObjectManager<GL::VertexArray> vao {};
    vao.handle().bind();

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    const Vertex vertices[] {
            {{ .5, .5,-.5}, { .0, .0,-1.}}, // 0
            {{ .5,-.5,-.5}, { .0, .0,-1.}},
            {{-.5, .5,-.5}, { .0, .0,-1.}},
            {{-.5,-.5,-.5}, { .0, .0,-1.}},

            {{ .5, .5, .5}, { .0, .0, 1.}}, // 4
            {{ .5,-.5, .5}, { .0, .0, 1.}},
            {{-.5, .5, .5}, { .0, .0, 1.}},
            {{-.5,-.5, .5}, { .0, .0, 1.}},

            {{ .5, .5, .5}, { 0., 1., 0.}}, // 8
            {{ .5, .5,-.5}, { 0., 1., 0.}},
            {{-.5, .5, .5}, { 0., 1., 0.}},
            {{-.5, .5,-.5}, { 0., 1., 0.}},

            {{ .5,-.5, .5}, { 0.,-1., 0.}}, // 12
            {{ .5,-.5,-.5}, { 0.,-1., 0.}},
            {{-.5,-.5, .5}, { 0.,-1., 0.}},
            {{-.5,-.5,-.5}, { 0.,-1., 0.}},

            {{ .5, .5, .5}, { 1., 0., 0.}}, // 16
            {{ .5, .5,-.5}, { 1., 0., 0.}},
            {{ .5,-.5, .5}, { 1., 0., 0.}},
            {{ .5,-.5,-.5}, { 1., 0., 0.}},

            {{-.5, .5, .5}, {-1., 0., 0.}}, // 20
            {{-.5, .5,-.5}, {-1., 0., 0.}},
            {{-.5,-.5, .5}, {-1., 0., 0.}},
            {{-.5,-.5,-.5}, {-1., 0., 0.}},
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

    glm::vec3 camera_position {3.0f};
    glm::mat4 view_mtx = glm::lookAt(camera_position, glm::vec3(0.0f), {0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mtx = glm::perspectiveFov(70.0f, 1024.0f, 768.0f, 0.01f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    shader_program.handle().useProgram();

    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view_mtx));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(proj_mtx));
    glUniform3fv(7, 1, glm::value_ptr(camera_position));

    while (!glfwWindowShouldClose(window.ptr())) {
        constexpr float angular_speed = 1.0f;
        float angle = angular_speed * glfwGetTime();
        glm::mat4 model_mtx = glm::rotate(glm::mat4(1.0f), angle, {0.0f, 1.0f, 0.0f});
        model_mtx = glm::rotate(model_mtx, angle, {0.0f, 0.0f, 1.0f});
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(model_mtx));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window.ptr());
        glfwPollEvents();
    }

    return 0;
}