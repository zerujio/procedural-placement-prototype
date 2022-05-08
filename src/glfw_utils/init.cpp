#include "init.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace GLFW {

InitSentry::InitSentry() {
    if (!glfwInit()) {
        const char *errmsg;
        glfwGetError(&errmsg);
        throw std::runtime_error(errmsg);
    }
}

InitSentry::~InitSentry() {
    glfwTerminate();
}

} // namespace GLFW
