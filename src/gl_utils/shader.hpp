#ifndef GL_UTILS_SHADER_HPP
#define GL_UTILS_SHADER_HPP

#include <glad/glad.h>

#include <string>

#include "object_manager.hpp"

namespace GL {

enum class ShaderType {
  Vertex    = GL_VERTEX_SHADER,
  Fragment  = GL_FRAGMENT_SHADER,
  Compute   = GL_COMPUTE_SHADER
}

// Shader handle
class Shader {

  friend class ObjectManager<Shader>;

public:

  // create a null (invalid) shader handle
  Shader() = default;

  // create a new shader object and return its id
  static Shader create();

  // delete the shader object with the given id
  static void delete(Shader shader);

  // set shader source code
  void setSource(std::string src) const;

  // check validity of shader id
  void valid() const;

protected:
  
  Shader(GLuint id) : m_id(id) {}

private:

  GLuint m_id {0};

};

} // namespace GL

#endif//GL_UTILS_SHADER_HPP
