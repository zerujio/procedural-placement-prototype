#include "glfw_window.hpp"

#include <stdexcept>

namespace GLFW {

  Window::Window(GLFWwindow* window_ptr)
  : m_window_ptr(window_ptr)
  {}

  Window::Window(unsigned int width, unsigned int height, const char* window_title)
  : m_window_ptr(glfwCreateWindow(width, height, window_title, nullptr, nullptr))
  {
    if (!m_window_ptr){
      const char* error_msg;
      glfwGetError(&error_msg);
      throw std::runtime_error(error_msg);
    }
  }
  
  Window::Window(Window&& other)
  : m_window_ptr(other.m_window_ptr)
  {
    other.m_window_ptr = nullptr;
  }

  Window& Window::operator= (Window&& other)
  {
    m_window_ptr = other.m_window_ptr;
    other.m_window_ptr = nullptr;

    return *this;
  }

  Window::~Window()
  {
    if (m_window_ptr)
      glfwDestroyWindow(m_window_ptr);
  }

  GLFWwindow* Window::ptr() const {
    return m_window_ptr;
  }

} // namespace GLFW
