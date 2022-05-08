#ifndef GL_UTILS_SHADER_HPP
#define GL_UTILS_SHADER_HPP

#include <glad/glad.h>

#include <string>
#include <vector>

#include "object_manager.hpp"

namespace GL {

enum class ShaderType {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Compute = GL_COMPUTE_SHADER
};

// Shader handle
class Shader {

    friend class ShaderProgram;

public:

    // create a null (invalid) shader handle
    Shader() = default;

    // create a new shader object and return its id
    static Shader create(ShaderType shader_type);

    // delete the shader object with the given id
    static void destroy(Shader shader);

    // set shader source code
    void setSource(const std::string &src) const;

    void setSource(const std::vector<std::string> &src_vector) const;

    void compileShader() const;

    // check validity of shader id
    bool valid() const;

protected:

    Shader(GLuint id) : m_id(id) {}

private:

    GLuint m_id{0};

};

} // namespace GL

#endif//GL_UTILS_SHADER_HPP
