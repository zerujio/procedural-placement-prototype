#include "shader_program.hpp"

#include <iostream>

namespace GL {


ShaderProgram ShaderProgram::create() {
    return {glCreateProgram()};
}

void ShaderProgram::destroy(ShaderProgram program) {
    glDeleteProgram(program.m_id);
}

bool ShaderProgram::valid() const {
    return glIsProgram(m_id);
}

void ShaderProgram::attachShader(Shader shader) const {
    glAttachShader(m_id, shader.m_id);
}

void ShaderProgram::detachShader(Shader shader) const {
    glDetachShader(m_id, shader.m_id);
}

void ShaderProgram::linkProgram() const {
    std::cout << "Linking program [id=" << m_id << "] ... ";

    glLinkProgram(m_id);

    GLint success, log_length;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &log_length);

    std::cout << (success ? "SUCCESS" : "ERROR") << std::endl;
    if (log_length) {
        char buf[log_length];
        glGetProgramInfoLog(m_id, log_length, nullptr, buf);
        std::cout << buf << std::endl;
    }
}

void ShaderProgram::useProgram() const {
    glUseProgram(m_id);
}

GLuint ShaderProgram::id() const {
    return m_id;
}

GLint ShaderProgram::getUniformLocation(const char *name) const {
    return glGetUniformLocation(m_id, name);
}

GLint ShaderProgram::getAttribLocation(const char *name) const {
    return glGetAttribLocation(m_id, name);
}

} // GL