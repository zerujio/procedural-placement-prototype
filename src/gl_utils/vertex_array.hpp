#ifndef SRC_GL_UTILS__VERTEX_ARRAY_HPP
#define SRC_GL_UTILS__VERTEX_ARRAY_HPP

#include <glad/glad.h>

#include "buffer.hpp"

namespace GL {

class VertexArray {

public:

    VertexArray() = default;

    static VertexArray create();

    static void destroy(VertexArray va);

    [[nodiscard]] bool valid() const;

    void bind() const;

    void bindVertexBuffer(GLuint index, GL::Buffer buffer, GLintptr offset, GLsizei stride) const;

    void bindVertexBuffers(GLuint first_index, GLsizei count,
                           const GL::Buffer buffers[],
                           const GLintptr offsets[],
                           const GLsizei strides[]) const;

    void attribBinding(GLuint attrib_index, GLuint binding_index) const;

    void attribFormat(GLuint attrib_index, GLint size, GLenum type, bool normalized, GLuint relative_offset) const;

    void bindingDivisor(GLuint binding_index, GLuint divisor) const;

    void bindElementBuffer(GL::Buffer buffer) const;

    void enableAttrib(GLuint index) const;

private:

    VertexArray(GLuint id) : m_id(id) {}

    GLuint m_id {0};

};

} // GL

#endif //SRC_GL_UTILS__VERTEX_ARRAY_HPP
