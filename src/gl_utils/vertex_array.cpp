#include "vertex_array.hpp"

namespace GL {
VertexArray VertexArray::create() {
    VertexArray vao {};
    glCreateVertexArrays(1, &vao.m_id);
    return vao;
}

void VertexArray::destroy(VertexArray va) {
    glDeleteVertexArrays(1, &va.m_id);
}

bool VertexArray::valid() const {
    return glIsVertexArray(m_id);
}

void VertexArray::bind() const {
    glBindVertexArray(m_id);
}

void VertexArray::bindVertexBuffer(GLuint index, GL::Buffer buffer, GLintptr offset, GLsizei stride) const {
    glVertexArrayVertexBuffer(m_id, index, buffer.id(), offset, stride);
}

void VertexArray::bindVertexBuffers(GLuint first_index, GLsizei count,
                                    const GL::Buffer *buffers,
                                    const GLintptr *offsets,
                                    const GLsizei *strides) const {
    glVertexArrayVertexBuffers(m_id, first_index, count, reinterpret_cast<const GLuint*>(buffers), offsets, strides);
}

void VertexArray::attribBinding(GLuint attrib_index, GLuint binding_index) const {
    glVertexArrayAttribBinding(m_id, attrib_index, binding_index);
}

void VertexArray::attribFormat(GLuint attrib_index, GLint size, GLenum type, bool normalized, GLuint relative_offset) const {
    glVertexArrayAttribFormat(m_id, attrib_index, size, type, normalized, relative_offset);
}

void VertexArray::bindElementBuffer(GL::Buffer buffer) const {
    glVertexArrayElementBuffer(m_id, buffer.id());
}

void VertexArray::bindingDivisor(GLuint binding_index, GLuint divisor) const {
    glVertexArrayBindingDivisor(m_id, binding_index, divisor);
}

    void VertexArray::enableAttrib(GLuint index) const {
        glEnableVertexArrayAttrib(m_id, index);
    }

} // GL