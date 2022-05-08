#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glfw_utils/window.hpp"
#include "glfw_utils/init.hpp"

#include "gl_utils/gl.hpp"

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
            std::fstream file {"shaders/simple.frag"};
            vshader_mngr.handle().setSource((std::stringstream() << file.rdbuf()).str());
        }
        shader_program.handle().attachShader(vshader_mngr.handle());

        GL::ObjectManager<GL::Shader> fshader_mngr {GL::ShaderType::Fragment};
        {
            std::fstream file {"shaders/simple.vert"};
            fshader_mngr.handle().setSource((std::stringstream() << file.rdbuf()).str());
        }
        shader_program.handle().attachShader(fshader_mngr.handle());
    }
    shader_program.handle().linkProgram();

    glClearColor(0.2f, 0.1f, 0.2f, 1.0f);

    while (!glfwWindowShouldClose(window.ptr())) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window.ptr());
        glfwPollEvents();
    }

    return 0;
}
