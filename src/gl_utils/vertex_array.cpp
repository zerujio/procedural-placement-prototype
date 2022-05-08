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


} // GL