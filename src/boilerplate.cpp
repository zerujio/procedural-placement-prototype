#include "demo.hpp"

#include "glfw_utils/window.hpp"
#include "glfw_utils/init.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>

constexpr glm::uvec2 initial_window_size {1024, 768};

Scene *g_scene;
double g_delta;

void glfwCallback(int error_code, const char *description) {
    std::cerr << "[GLFW ERROR " << error_code << "] " << description << std::endl;
}

void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        glfwSetInputMode(w, GLFW_CURSOR, action == GLFW_PRESS ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        glfwSetInputMode(w, GLFW_RAW_MOUSE_MOTION, action == GLFW_PRESS);
    }
}

void frameBufferSizeCallback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
    g_scene->frameBufferSizeCallback(w, g_delta, {width, height});
}

void scrollCallback(GLFWwindow* w, double x, double y) {
    g_scene->scrollCallback(w, g_delta, {x, y});
}

struct ImGuiInitSentry {
    ImGuiInitSentry(GLFWwindow* w) {
        // ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        //ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(w, true);
        ImGui_ImplOpenGL3_Init("#version 150");
    }

    ~ImGuiInitSentry() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

int main() {
    glfwSetErrorCallback(glfwCallback);

    GLFW::InitSentry GLFW_init_sentry;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    GLFW::Window window {initial_window_size.x, initial_window_size.y, "Hello world!"};

    glfwMakeContextCurrent(window.ptr());
    glfwSetMouseButtonCallback(window.ptr(), mouseButtonCallback);
    glfwSetScrollCallback(window.ptr(), scrollCallback);
    glfwSetFramebufferSizeCallback(window.ptr(), frameBufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "OpenGL initialization failed" << std::endl;
        return -1;
    }

    GL::enableDebugMessageCallback();

    Scene scene;
    g_scene = &scene;

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    ImGuiInitSentry ImGui_init_sentry(window.ptr());

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(window.ptr())) {
        double now = glfwGetTime();
        g_delta = now - last;
        last = now;

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        scene.update(window.ptr(), g_delta);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.ptr());
    }
}