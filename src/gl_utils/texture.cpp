#include "texture.hpp"

namespace GL {

Texture Texture::create() {
    Texture texture;
    glGenTextures(1, &texture.m_id);
    return texture;
}

void Texture::destroy(Texture texture) {
    glDeleteTextures(1, &texture.m_id);
}

bool Texture::valid() const {
    return glIsTexture(m_id);
}

void Texture::bind(Texture::Target target) const {
    glBindTexture(static_cast<GLenum>(target), m_id);
}

void Texture::unbind(Texture::Target target) {
    glBindTexture(static_cast<GLenum>(target), 0);
}

void Texture::texImage2D(Target target,
                         GLint level,
                         InternalFormat internal_format,
                         GLsizei width,
                         GLsizei height,
                         Format format,
                         Type type,
                         const void *data) {
    glTexImage2D(static_cast<GLenum>(target),
                 level,
                 static_cast<GLint>(internal_format),
                 width, height,
                 0, // unused arg
                 static_cast<GLenum>(format),
                 static_cast<GLenum>(type),
                 data);
}

void Texture::generateMipmap(Texture::Target target) {
    glGenerateMipmap(static_cast<GLuint>(target));
}

void Texture::setDepthStencilMode(Texture::Target target, Texture::DepthStencilMode mode) {
    glTexParameteri(static_cast<GLenum>(target), GL_DEPTH_STENCIL_TEXTURE_MODE, static_cast<GLint>(mode));
}

void Texture::setMinFilter(Texture::Target target, Texture::MinFilter filter_mode) {
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter_mode));
}

void Texture::setMaxFilter(Texture::Target target, Texture::MaxFilter max_filter) {
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(max_filter));
}

void Texture::setWrapMode(Texture::Target target, Texture::WrapAxis axis, Texture::WrapMode mode) {
    glTexParameteri(static_cast<GLenum>(target), static_cast<GLenum>(axis), static_cast<GLint>(mode));
}

void Texture::setActive(GLubyte i) {
    glActiveTexture(GL_TEXTURE0 + i);
}


} // namespace GL
