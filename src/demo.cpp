#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glfw_utils/window.hpp"
#include "glfw_utils/init.hpp"

#include "gl_utils/gl.hpp"

#include <glm/vec3.hpp>

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

    glm::vec3 vertices[] {
        {.5, .5, .0},
        {-.5, .5, .0},
        {.5, -.5, .0},
        {-.5, -.5, .0}
    };

    GL::ObjectManager<GL::Buffer> vbo_manager {};
    auto vbo = vbo_manager.handle();
    vbo.bind(GL::Buffer::Target::Array);
    GL::Buffer::setData(GL::Buffer::Target::Array, sizeof(vertices), vertices, GL::Buffer::Usage::StaticDraw);

    GL::ObjectManager<GL::VertexArray> vao {};
    vao.handle().bind();

    constexpr unsigned int a_position_index = 0;
    glVertexAttribPointer(a_position_index, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(a_position_index);


    unsigned int indices[] {
        0, 1, 3,
        0, 3, 2
    };

    GL::ObjectManager<GL::Buffer> ebo_manager {};
    auto ebo = ebo_manager.handle();
    ebo.bind(GL::Buffer::Target::ElementArray);
    GL::Buffer::setData(GL::Buffer::Target::ElementArray, sizeof(indices), indices, GL::Buffer::Usage::StaticDraw);

    glClearColor(0.2f, 0.1f, 0.2f, 1.0f);

    shader_program.handle().useProgram();
    while (!glfwWindowShouldClose(window.ptr())) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window.ptr());
        glfwPollEvents();
    }

    return 0;
}
