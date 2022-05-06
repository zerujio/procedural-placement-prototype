#ifndef GLFW_INIT_HPP
#define GLFW_INIT_HPP

namespace GLFW {

  struct InitSentry {
    InitSentry();
    ~InitSentry();

    InitSentry(const InitSentry&) = delete;
    InitSentry& operator=(const InitSentry&) = delete;
  };

} // namespace GLFW

#endif//GLFW_INIT_HPP
