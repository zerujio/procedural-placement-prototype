#include "shader_program.hpp"

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
    glLinkProgram(m_id);
}

void ShaderProgram::useProgram() const {
    glUseProgram(m_id);
}


} // GL