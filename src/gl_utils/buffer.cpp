#include "buffer.hpp"

namespace GL {

    Buffer Buffer::create() {
        Buffer buffer {};
        glCreateBuffers(1, &buffer.m_id);
        return buffer;
    }

    void Buffer::destroy(Buffer buffer) {
        glDeleteBuffers(1, &buffer.m_id);
    }

    bool Buffer::valid() const {
        return glIsBuffer(m_id);
    }

    void Buffer::bind(Buffer::Target target) const {
        glBindBuffer(static_cast<GLenum>(target), m_id);
    }

    void Buffer::allocate(GLsizeiptr size, Usage usage) const {
        glNamedBufferData(id(), size, nullptr, static_cast<GLenum>(usage));
    }

    void Buffer::initialize(GLsizeiptr size, const void *data, Usage usage) const {
        glNamedBufferData(id(), size, data, static_cast<GLenum>(usage));
    }

    void Buffer::writeData(GLintptr offset, GLsizeiptr size, const void *data) const {
        glNamedBufferSubData(id(), offset, size, data);
    }

    GLuint Buffer::id() const {
        return m_id;
    }

} // GL