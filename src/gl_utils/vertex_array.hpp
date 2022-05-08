#ifndef SRC_GL_UTILS__VERTEX_ARRAY_HPP
#define SRC_GL_UTILS__VERTEX_ARRAY_HPP

#include <glad/glad.h>

namespace GL {

class VertexArray {

public:

    VertexArray() = default;

    static VertexArray create();

    static void destroy(VertexArray va);

    [[nodiscard]] bool valid() const;

    void bind() const;

private:

    VertexArray(GLuint id) : m_id(id) {}

    GLuint m_id {0};

};

} // GL

#endif //SRC_GL_UTILS__VERTEX_ARRAY_HPP
