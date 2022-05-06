#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glfw_utils/window.hpp"
#include "glfw_utils/init.hpp"

#include <iostream>

int main() {

  GLFW::InitSentry _glfw_init_sentry;

  GLFW::Window window {1024, 768, "Hello world!"};

  glfwMakeContextCurrent(window.ptr());

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cerr << "OpenGL initialization failed" << std::endl;
    return -1;
  }

  glClearColor(0.2f, 0.1f, 0.2f, 1.0f);

  while (!glfwWindowShouldClose(window.ptr())) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window.ptr());
    glfwPollEvents();
  }

  return 0;
}
