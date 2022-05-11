#ifndef SRC_GL_UTILS__BUFFER_HPP
#define SRC_GL_UTILS__BUFFER_HPP

#include <glad/glad.h>

#include <vector>

namespace GL {

class Buffer {

public:

    enum class Target {
        Array = GL_ARRAY_BUFFER,
        ElementArray = GL_ELEMENT_ARRAY_BUFFER,
        ShaderStorage = GL_SHADER_STORAGE_BUFFER
    };

    enum class Usage {
        StreamDraw = GL_STREAM_DRAW,
        StaticDraw = GL_STATIC_DRAW,
        DynamicDraw = GL_DYNAMIC_DRAW,
    };

    Buffer() = default;

    static Buffer create();

    static void destroy(Buffer buffer);

    [[nodiscard]] bool valid() const;

    void bind(Target target) const;

    static void setData(Target target, GLsizeiptr size, const void *data, Usage usage);

    template<class Container>
    static void setData(Target target, Container data, Usage usage) {
        setData(target, data.size() * sizeof(Container::value_type), data.data(), usage);
    }

    [[nodiscard]] GLuint id() const;

private:

    Buffer(GLuint id) : m_id(id) {}

    GLuint m_id;

};

} // GL

#endif //SRC_GL_UTILS__BUFFER_HPP
