#include "texture_load.hpp"

#include "image.hpp"

GL::Texture loadTexture(const char* file_path) {
    Image image(file_path);

    using IFormat = GL::Texture::InternalFormat;
    using Format = GL::Texture::Format;
    using Type = GL::Texture::Type;

    const std::pair<IFormat, Format> formats[] {
            {IFormat::R8, Format::Red},
            {IFormat::RG8, Format::RG},
            {IFormat::RGB8, Format::RGB},
            {IFormat::RGBA8, Format::RGBA}
    };

    auto iformat = formats[image.channels() - 1].first;
    auto format = formats[image.channels() - 1].second;

    auto texture = GL::Texture::create();
    auto target = GL::Texture::Target::Tex2D;
    texture.bind(target);
    GL::Texture::texImage2D(target, 0,
                            iformat,
                            image.dimensions().x, image.dimensions().y,
                            format,
                            Type::UByte,
                            image.data());
    GL::Texture::generateMipmap(target);

    return {texture};
}