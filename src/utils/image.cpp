#include "image.hpp"

#include <stb_image.h>

#include <stdexcept>

//namespace Folk {

Image::Image(const char *file_name)
: p_data(stbi_load(file_name, &m_dims.x, &m_dims.y, &m_channels, 0))
{
    if (!p_data)
        throw std::runtime_error(std::string("Image load failed: ") + stbi_failure_reason());
}

Image::Image(const std::string &file_name) : Image(file_name.c_str()) {}

const unsigned char *Image::data() const {
    return p_data.get();
}

glm::ivec2 Image::dimensions() const {
    return m_dims;
}

int Image::channels() const {
    return m_channels;
}

void Image::Deleter::operator() (Image::byte *data) const {
    stbi_image_free(data);
}


//} // namespace Folk
