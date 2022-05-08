#include "gl.hpp"

#include <iostream>

namespace GL {

void debugCallback(GLenum source,
                   GLenum type,
                   GLuint id,
                   GLenum severity,
                   GLsizei length,
                   const GLchar *message,
                   const void *userParam) {
    const char *source_string;
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            source_string = "OpenGL API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            source_string = "Window-system API";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            source_string = "Shader compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            source_string = "Third-party application";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            source_string = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            source_string = "Other";
            break;
        default:
            source_string = "Unknown/Invalid";
            break;
    }

    const char *type_string;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_string = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_string = "Deprecated behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_string = "Undefined behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_string = "Portability warning";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_string = "Performance warning";
            break;
        case GL_DEBUG_TYPE_MARKER:
            type_string = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            type_string = "Push group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            type_string = "Pop group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            type_string = "Other";
            break;
        default:
            type_string = "Unknown/Invalid";
            break;
    }

    const char *severity_string;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            severity_string = "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severity_string = "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severity_string = "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severity_string = "Notification";
            break;
        default:
            severity_string = "Unknown/Invalid";
            break;
    }

    std::cout << "\n[OpenGL DEBUG MESSAGE]\n"
              << "Source  :" << source_string << '\n'
              << "Type    :" << type_string << '\n'
              << "Severity:" << severity_string << '\n'
              << message << std::endl;
}

void enableDebugMessageCallback() {
    glDebugMessageCallback(debugCallback, nullptr);
}

} // namespace GL