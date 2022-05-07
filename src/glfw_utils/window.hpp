#ifndef GLFW_WINDOW_HPP
#define GLFW_WINDOW_HPP

#include <GLFW/glfw3.h>

namespace GLFW {

  class Window final {

  public:
    Window() = default;

    Window(GLFWwindow* window_ptr);

    Window(unsigned int width, unsigned int height, const char* window_title);

    Window(Window&& other);

    ~Window();

    Window& operator= (Window&& other);

    GLFWwindow* ptr() const;

  private:
    GLFWwindow* m_window_ptr {nullptr};

  };

} // namespace GLFW

#endif//GLFW_WINDOW_HPP
