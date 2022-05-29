#ifndef SRC_GL_UTILS__SHADER_PROGRAM_HPP
#define SRC_GL_UTILS__SHADER_PROGRAM_HPP

#include "shader.hpp"

namespace GL {


class ShaderProgram {

public:
    ShaderProgram() = default;

    static ShaderProgram create();

    static void destroy(ShaderProgram program);

    [[nodiscard]] bool valid() const;

    void attachShader(Shader shader) const;

    void detachShader(Shader shader) const;

    void linkProgram() const;

    void useProgram() const;

    GLint getUniformLocation(const char* name) const;

    GLint getAttribLocation(const char* name) const;

    [[nodiscard]] GLuint id() const;

protected:

    ShaderProgram(GLuint id) : m_id(id) {}

private:

    GLuint m_id {0};

};

} // GL

#endif //SRC_GL_UTILS__SHADER_PROGRAM_HPP
