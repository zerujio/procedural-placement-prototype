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

    /// Allocate uninitialized memory.
    void allocate(GLsizeiptr size, Usage usage) const;

    /// Allocate memory and initialize it to the contents of @p data.
    void initialize(GLsizeiptr size, const void *data, Usage usage) const;

    /// Write data to vertex_buffer starting at @p offset (measured in bytes).
    void writeData(GLintptr offset, GLsizeiptr size, const void *data) const;

    /**
     * @brief Allocate memory and initialize it to the contents of @p data.
     * @param data A contiguous container, such as std::vector.
     * @param usage OpenGL vertex_buffer usage hint.
     */
    template<class Container>
    void initialize(Container data, Usage usage) const {
        auto alignment = std::alignment_of_v<typename Container::value_type>;
        initialize(data.size() * alignment, data.data(), usage);
    }

    [[nodiscard]] GLuint id() const;

private:

    Buffer(GLuint id) : m_id(id) {}

    GLuint m_id;

};

} // GL

#endif //SRC_GL_UTILS__BUFFER_HPP
