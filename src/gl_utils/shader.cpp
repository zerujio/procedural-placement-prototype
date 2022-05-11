#include "shader.hpp"

#include <vector>
#include <stdexcept>
#include <iostream>

namespace GL {

Shader Shader::create(ShaderType shader_type) {
    return {glCreateShader(static_cast<GLenum>(shader_type))};
}

void Shader::destroy(Shader shader) {
    glDeleteShader(shader.m_id);
}

void Shader::setSource(const std::string &src) const {
    auto c_str = src.c_str();
    glShaderSource(m_id, 1, &c_str, nullptr);
}

void Shader::setSource(const std::vector<std::string> &src_vector) const {
    if (src_vector.empty())
        throw std::logic_error("Shader::setSource given empty source vector");

    const char *sources[src_vector.size()];
    for (int i = 0; i < src_vector.size(); ++i) {
        sources[i] = src_vector[i].c_str();
    }

    glShaderSource(m_id, src_vector.size(), sources, nullptr);
}

void Shader::compileShader() const {
    std::cout << "Compiling shader [id=" << m_id << "] ... ";
    glCompileShader(m_id);

    GLint success, log_length;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &log_length);

    std::cout << ((success) ? "SUCCESS" : "ERROR") << std::endl;
    if (log_length > 0) {
        char buf[log_length];
        glGetShaderInfoLog(m_id, log_length, nullptr, buf);
        std::cout << buf << std::endl;
    }
}

bool Shader::valid() const {
    return glIsShader(m_id);
}

GLuint Shader::id() const {
    return m_id;
}

} // namespace GL