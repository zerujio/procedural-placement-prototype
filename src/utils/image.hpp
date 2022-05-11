#ifndef SRC_RENDER__IMAGE_HPP
#define SRC_RENDER__IMAGE_HPP

#include <glm/vec2.hpp>

#include <string>
#include <memory>

//namespace Folk {

class Image final {

public:
    /// Load image from a file.
    explicit Image(const char* file_name);
    explicit Image(const std::string& file_name);

    /**
     * @brief Access image buffer.
     * @return Pointer raw image data.
     */
    [[nodiscard]] const unsigned char* data() const;

    /// Image dimensions in pixels.
    [[nodiscard]] glm::ivec2 dimensions() const;

    /// Number of color channels in the image.
    [[nodiscard]] int channels() const;

private:

    using byte = unsigned char;

    struct Deleter {
        void operator() (byte* data) const;
    };

    glm::ivec2 m_dims;
    int m_channels;
    std::unique_ptr<unsigned char, Deleter> p_data;
};

//} // namespace Folk

#endif //SRC_RENDER__IMAGE_HPP
