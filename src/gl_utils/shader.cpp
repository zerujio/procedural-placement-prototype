#include "shader.hpp"

#include <ifstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string& filename, ShaderType type)
: m_id (glCreateShader(static_cast<GLint>(type)))
{
  if (!m_id) {
    throw std::runtime_error("Shader creation failed");
  }

  std::ifstream file {filename};
  std::string str = (std::stringstream() << file.rdbuf()).str();
  const char* c_str = str.c_str();

  glShaderSource(m_id, 1, &c_str, nullptr);
  glCompileShader(m_id);
}

Shader::Shader(Shader &&other) :
m_id(other.m_id)
{
  other.m_id = 0;
}
