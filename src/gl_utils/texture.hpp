#ifndef GL_UTILS_TEXTURE_HPP
#define GL_UTILS_TEXTURE_HPP

#include <glad/glad.h>

//namespace Folk::gl {
namespace GL {

class Texture {

public:

    Texture() = default;

    static Texture create();

    static void destroy(Texture texture);

    [[nodiscard]] bool valid() const;

    /// Sets the active texture unit to @param i.
    static void setActive(GLubyte i);

    /// Texture binding targets.
    enum class Target : GLenum {
        Tex1D = GL_TEXTURE_1D,
        Tex2D = GL_TEXTURE_2D,
        Tex3D = GL_TEXTURE_3D,
        Tex1DArray  = GL_TEXTURE_1D_ARRAY,
        Tex2DArray  = GL_TEXTURE_2D_ARRAY,
        Rectangle   = GL_TEXTURE_RECTANGLE,
        CubeMap     = GL_TEXTURE_CUBE_MAP,
        CubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
        TexBuffer   = GL_TEXTURE_BUFFER,
        Tex2DMultisample    = GL_TEXTURE_2D_MULTISAMPLE,
        Tex2DMultisampleArray   = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    };

    void bind(Target target) const;

    static void unbind(Target target);

    /// Format used by OpenGL to store texture data
    enum class InternalFormat : GLenum {
        R8 = GL_R8,
        R8SNorm = GL_R8_SNORM,
        R16 = GL_R16,
        R16SNorm = GL_R16_SNORM,
        RG8 = GL_RG8,
        RG8SNorm = GL_RG8_SNORM,
        RG16 = GL_RG16,
        RG16SNorm = GL_RG16_SNORM,
        R3_G3_B2 = GL_R3_G3_B2,
        RGB4 = GL_RGB4,
        RGB5 = GL_RGB5,
        RGB8 = GL_RGB8,
        RGB8SNorm = GL_RGB8_SNORM,
        RGB10 = GL_RGB10,
        RGB12 = GL_RGB12,
        RGB16SNorm = GL_RGB16_SNORM,
        RGBA2 = GL_RGBA2,
        RGBA4 = GL_RGBA4,
        RGB5_A1 = GL_RGB5_A1,
        RGBA8 = GL_RGBA8,
        RGBA8SNorm = GL_RGBA8_SNORM,
        RGB10_A2 = GL_RGB10_A2,
        RGB10_A2UI = GL_RGB10_A2UI,
        RGBA12 = GL_RGBA12,
        RGBA16 = GL_RGBA16,
        SRGB8 = GL_SRGB8,
        SRGB8_ALPHA8 = GL_SRGB8_ALPHA8,
        R16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        R32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,
        R11F_G11F_B10F = GL_R11F_G11F_B10F,
        RGB9_E5 = GL_RGB9_E5,
        R8I = GL_R8I,
        R8UI = GL_R8UI,
        R16I = GL_R16I,
        R16UI = GL_R16UI,
        R32I = GL_R32I,
        R32UI = GL_R32UI,
        RG8I = GL_RG8I,
        RG8UI = GL_RG8UI,
        RG16I = GL_RG16I,
        RG16UI = GL_RG16UI,
        RG32I = GL_RG32I,
        RG32UI = GL_RG32UI,
        RGB8I = GL_RGB8I,
        RGB8UI = GL_RGB8UI,
        RGB16I = GL_RGB16I,
        RGB16UI = GL_RGB16UI,
        RGB32I = GL_RGB32I,
        RGB32UI = GL_RGB32UI,
        RGBA8I = GL_RGBA8I,
        RGBA8UI = GL_RGBA8UI,
        RGBA16I = GL_RGBA16I,
        RGBA16UI = GL_RGBA16UI,
        RGBA32I = GL_RGBA32I,
        RGBA32UI = GL_RGBA32UI,
    };

    enum class Format : GLenum {
        Red = GL_RED,
        RG  = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,
        RedInt  = GL_RED_INTEGER,
        RGInt   = GL_RG_INTEGER,
        RGBInt  = GL_RGB_INTEGER,
        BGRInt  = GL_BGR_INTEGER,
        RGBAInt = GL_RGBA_INTEGER,
        BGRAInt = GL_BGRA_INTEGER,
        StencilIndex    = GL_STENCIL_INDEX,
        DepthComponent  = GL_DEPTH_COMPONENT,
        DepthStencil    = GL_DEPTH_STENCIL
    };

    enum class Type : GLenum {
        UByte   = GL_UNSIGNED_BYTE,
        Byte    = GL_BYTE,
        UShort  = GL_UNSIGNED_SHORT,
        Short   = GL_SHORT,
        UInt    = GL_UNSIGNED_INT,
        Int     = GL_INT,
        HalfFloat   = GL_HALF_FLOAT,
        Float       = GL_FLOAT,
        UByte_3_3_2         = GL_UNSIGNED_BYTE_3_3_2,
        UByte_2_3_3_Rev     = GL_UNSIGNED_BYTE_2_3_3_REV,
        UShort_5_6_5        = GL_UNSIGNED_SHORT_5_6_5,
        UShort_5_6_5_Rev    = GL_UNSIGNED_SHORT_5_6_5_REV,
        UShort_4_4_4_4      = GL_UNSIGNED_SHORT_4_4_4_4,
        UShort_4_4_4_4_Rev  = GL_UNSIGNED_SHORT_4_4_4_4_REV,
        UShort_5_5_5_1      = GL_UNSIGNED_SHORT_5_5_5_1,
        UShort_1_5_5_5_Rev  = GL_UNSIGNED_SHORT_1_5_5_5_REV,
        UInt_8_8_8_8        = GL_UNSIGNED_INT_8_8_8_8,
        UInt_8_8_8_8_Rev    = GL_UNSIGNED_INT_8_8_8_8_REV,
        UInt_10_10_10_2     = GL_UNSIGNED_INT_10_10_10_2,
        UInt_2_10_10_10_Rev = GL_UNSIGNED_INT_2_10_10_10_REV
    };

    /**
     * @brief Write data into a 2D texture
     * @param target The texture target
     * @param level Mipmap level (0 is base).
     * @param internal_format Number of color components in image.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param format Format of data.
     * @param type Data type of data.
     * @param data Pointer to image data.
     */
    static void texImage2D(Target target,
                           GLint level,
                           InternalFormat internal_format,
                           GLsizei width,
                           GLsizei height,
                           Format format,
                           Type type,
                           const void* data);

    static void generateMipmap(Target target);

    enum class DepthStencilMode : GLenum {
        DepthComponent  = GL_DEPTH_COMPONENT,
        StencilIndex    = GL_STENCIL_INDEX,
    };

    static void setDepthStencilMode(Target target, DepthStencilMode mode);

    enum class MinFilter : GLenum {
        Nearest     = GL_NEAREST,
        Linear      = GL_LINEAR,
        NearestMipmapNearest    = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest     = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear     = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapLinear      = GL_LINEAR_MIPMAP_LINEAR
    };

    static void setMinFilter(Target target, MinFilter filter_mode);

    enum class MaxFilter : GLenum {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR
    };

    static void setMaxFilter(Target target, MaxFilter max_filter);

    enum class WrapMode : GLenum {
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
    };

    enum class WrapAxis : GLenum {
        S = GL_TEXTURE_WRAP_S,
        T = GL_TEXTURE_WRAP_T,
        R = GL_TEXTURE_WRAP_R,
    };

    static void setWrapMode(Target target, WrapAxis axis, WrapMode mode);

private:
    explicit Texture(GLuint id) : m_id(id) {}

    GLuint m_id {0};
};

} // namespace GL


#endif //GL_UTILS_TEXTURE_HPP
