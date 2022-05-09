#include "buffer.hpp"

namespace GL {


Buffer Buffer::create() {
    Buffer buffer {};
    glGenBuffers(1, &buffer.m_id);
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

void Buffer::setData(Buffer::Target target, GLsizeiptr size, const void *data, Buffer::Usage usage) {
    glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
}

} // GL