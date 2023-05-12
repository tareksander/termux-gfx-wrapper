#pragma once

#include <stdint.h>

extern "C" {

// definition from android/rect.h

/**
 * Rectangular window area.
 *
 * This is the NDK equivalent of the android.graphics.Rect class in Java. It is
 * used with {@link ANativeActivityCallbacks::onContentRectChanged} event
 * callback and the ANativeWindow_lock() function.
 *
 * In a valid ARect, left <= right and top <= bottom. ARect with left=0, top=10,
 * right=1, bottom=11 contains only one pixel at x=0, y=10.
 */
typedef struct ARect {
    /// Minimum X coordinate of the rectangle.
    int32_t left;
    /// Minimum Y coordinate of the rectangle.
    int32_t top;
    /// Maximum X coordinate of the rectangle.
    int32_t right;
    /// Maximum Y coordinate of the rectangle.
    int32_t bottom;
} ARect;


// definitions from android/hardware_buffer.h


/**
 * Buffer pixel formats.
 */
enum AHardwareBuffer_Format {
    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R8G8B8A8_UNORM
     *   OpenGL ES: GL_RGBA8
     */
    AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM           = 1,

    /**
     * 32 bits per pixel, 8 bits per channel format where alpha values are
     * ignored (always opaque).
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R8G8B8A8_UNORM
     *   OpenGL ES: GL_RGB8
     */
    AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM           = 2,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R8G8B8_UNORM
     *   OpenGL ES: GL_RGB8
     */
    AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM             = 3,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R5G6B5_UNORM_PACK16
     *   OpenGL ES: GL_RGB565
     */
    AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM             = 4,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_R16G16B16A16_SFLOAT
     *   OpenGL ES: GL_RGBA16F
     */
    AHARDWAREBUFFER_FORMAT_R16G16B16A16_FLOAT       = 0x16,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_A2B10G10R10_UNORM_PACK32
     *   OpenGL ES: GL_RGB10_A2
     */
    AHARDWAREBUFFER_FORMAT_R10G10B10A2_UNORM        = 0x2b,

    /**
     * Opaque binary blob format.
     * Must have height 1 and one layer, with width equal to the buffer
     * size in bytes. Corresponds to Vulkan buffers and OpenGL buffer
     * objects. Can be bound to the latter using GL_EXT_external_buffer.
     */
    AHARDWAREBUFFER_FORMAT_BLOB                     = 0x21,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D16_UNORM
     *   OpenGL ES: GL_DEPTH_COMPONENT16
     */
    AHARDWAREBUFFER_FORMAT_D16_UNORM                = 0x30,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_X8_D24_UNORM_PACK32
     *   OpenGL ES: GL_DEPTH_COMPONENT24
     */
    AHARDWAREBUFFER_FORMAT_D24_UNORM                = 0x31,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D24_UNORM_S8_UINT
     *   OpenGL ES: GL_DEPTH24_STENCIL8
     */
    AHARDWAREBUFFER_FORMAT_D24_UNORM_S8_UINT        = 0x32,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D32_SFLOAT
     *   OpenGL ES: GL_DEPTH_COMPONENT32F
     */
    AHARDWAREBUFFER_FORMAT_D32_FLOAT                = 0x33,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_D32_SFLOAT_S8_UINT
     *   OpenGL ES: GL_DEPTH32F_STENCIL8
     */
    AHARDWAREBUFFER_FORMAT_D32_FLOAT_S8_UINT        = 0x34,

    /**
     * Corresponding formats:
     *   Vulkan: VK_FORMAT_S8_UINT
     *   OpenGL ES: GL_STENCIL_INDEX8
     */
    AHARDWAREBUFFER_FORMAT_S8_UINT                  = 0x35,

    /**
     * YUV 420 888 format.
     * Must have an even width and height. Can be accessed in OpenGL
     * shaders through an external sampler. Does not support mip-maps
     * cube-maps or multi-layered textures.
     */
    AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420             = 0x23,
};

/**
 * Buffer usage flags, specifying how the buffer will be accessed.
 */
enum AHardwareBuffer_UsageFlags {
    /**
     * The buffer will never be locked for direct CPU reads using the
     * AHardwareBuffer_lock() function. Note that reading the buffer
     * using OpenGL or Vulkan functions or memory mappings is still
     * allowed.
     */
    AHARDWAREBUFFER_USAGE_CPU_READ_NEVER        = 0UL,
    /**
     * The buffer will sometimes be locked for direct CPU reads using
     * the AHardwareBuffer_lock() function. Note that reading the
     * buffer using OpenGL or Vulkan functions or memory mappings
     * does not require the presence of this flag.
     */
    AHARDWAREBUFFER_USAGE_CPU_READ_RARELY       = 2UL,
    /**
     * The buffer will often be locked for direct CPU reads using
     * the AHardwareBuffer_lock() function. Note that reading the
     * buffer using OpenGL or Vulkan functions or memory mappings
     * does not require the presence of this flag.
     */
    AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN        = 3UL,

    /** CPU read value mask. */
    AHARDWAREBUFFER_USAGE_CPU_READ_MASK         = 0xFUL,
    /**
     * The buffer will never be locked for direct CPU writes using the
     * AHardwareBuffer_lock() function. Note that writing the buffer
     * using OpenGL or Vulkan functions or memory mappings is still
     * allowed.
     */
    AHARDWAREBUFFER_USAGE_CPU_WRITE_NEVER       = 0UL << 4,
    /**
     * The buffer will sometimes be locked for direct CPU writes using
     * the AHardwareBuffer_lock() function. Note that writing the
     * buffer using OpenGL or Vulkan functions or memory mappings
     * does not require the presence of this flag.
     */
    AHARDWAREBUFFER_USAGE_CPU_WRITE_RARELY      = 2UL << 4,
    /**
     * The buffer will often be locked for direct CPU writes using
     * the AHardwareBuffer_lock() function. Note that writing the
     * buffer using OpenGL or Vulkan functions or memory mappings
     * does not require the presence of this flag.
     */
    AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN       = 3UL << 4,
    /** CPU write value mask. */
    AHARDWAREBUFFER_USAGE_CPU_WRITE_MASK        = 0xFUL << 4,
    /** The buffer will be read from by the GPU as a texture. */
    AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE     = 1UL << 8,
    /** The buffer will be written to by the GPU as a framebuffer attachment.*/
    AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER       = 1UL << 9,
    /**
     * The buffer will be written to by the GPU as a framebuffer
     * attachment.
     *
     * Note that the name of this flag is somewhat misleading: it does
     * not imply that the buffer contains a color format. A buffer with
     * depth or stencil format that will be used as a framebuffer
     * attachment should also have this flag. Use the equivalent flag
     * AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER to avoid this confusion.
     */
    AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT      = AHARDWAREBUFFER_USAGE_GPU_FRAMEBUFFER,
    /**
     * The buffer will be used as a composer HAL overlay layer.
     *
     * This flag is currently only needed when using ASurfaceTransaction_setBuffer
     * to set a buffer. In all other cases, the framework adds this flag
     * internally to buffers that could be presented in a composer overlay.
     * ASurfaceTransaction_setBuffer is special because it uses buffers allocated
     * directly through AHardwareBuffer_allocate instead of buffers allocated
     * by the framework.
     */
    AHARDWAREBUFFER_USAGE_COMPOSER_OVERLAY      = 1ULL << 11,
    /**
     * The buffer is protected from direct CPU access or being read by
     * non-secure hardware, such as video encoders.
     *
     * This flag is incompatible with CPU read and write flags. It is
     * mainly used when handling DRM video. Refer to the EGL extension
     * EGL_EXT_protected_content and GL extension
     * GL_EXT_protected_textures for more information on how these
     * buffers are expected to behave.
     */
    AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT     = 1UL << 14,
    /** The buffer will be read by a hardware video encoder. */
    AHARDWAREBUFFER_USAGE_VIDEO_ENCODE          = 1UL << 16,
    /**
     * The buffer will be used for direct writes from sensors.
     * When this flag is present, the format must be AHARDWAREBUFFER_FORMAT_BLOB.
     */
    AHARDWAREBUFFER_USAGE_SENSOR_DIRECT_DATA    = 1UL << 23,
    /**
     * The buffer will be used as a shader storage or uniform buffer object.
     * When this flag is present, the format must be AHARDWAREBUFFER_FORMAT_BLOB.
     */
    AHARDWAREBUFFER_USAGE_GPU_DATA_BUFFER       = 1UL << 24,
    /**
     * The buffer will be used as a cube map texture.
     * When this flag is present, the buffer must have a layer count
     * that is a multiple of 6. Note that buffers with this flag must be
     * bound to OpenGL textures using the extension
     * GL_EXT_EGL_image_storage instead of GL_KHR_EGL_image.
     */
    AHARDWAREBUFFER_USAGE_GPU_CUBE_MAP          = 1UL << 25,
    /**
     * The buffer contains a complete mipmap hierarchy.
     * Note that buffers with this flag must be bound to OpenGL textures using
     * the extension GL_EXT_EGL_image_storage instead of GL_KHR_EGL_image.
     */
    AHARDWAREBUFFER_USAGE_GPU_MIPMAP_COMPLETE   = 1UL << 26,

    AHARDWAREBUFFER_USAGE_VENDOR_0  = 1ULL << 28,
    AHARDWAREBUFFER_USAGE_VENDOR_1  = 1ULL << 29,
    AHARDWAREBUFFER_USAGE_VENDOR_2  = 1ULL << 30,
    AHARDWAREBUFFER_USAGE_VENDOR_3  = 1ULL << 31,
    AHARDWAREBUFFER_USAGE_VENDOR_4  = 1ULL << 48,
    AHARDWAREBUFFER_USAGE_VENDOR_5  = 1ULL << 49,
    AHARDWAREBUFFER_USAGE_VENDOR_6  = 1ULL << 50,
    AHARDWAREBUFFER_USAGE_VENDOR_7  = 1ULL << 51,
    AHARDWAREBUFFER_USAGE_VENDOR_8  = 1ULL << 52,
    AHARDWAREBUFFER_USAGE_VENDOR_9  = 1ULL << 53,
    AHARDWAREBUFFER_USAGE_VENDOR_10 = 1ULL << 54,
    AHARDWAREBUFFER_USAGE_VENDOR_11 = 1ULL << 55,
    AHARDWAREBUFFER_USAGE_VENDOR_12 = 1ULL << 56,
    AHARDWAREBUFFER_USAGE_VENDOR_13 = 1ULL << 57,
    AHARDWAREBUFFER_USAGE_VENDOR_14 = 1ULL << 58,
    AHARDWAREBUFFER_USAGE_VENDOR_15 = 1ULL << 59,
    AHARDWAREBUFFER_USAGE_VENDOR_16 = 1ULL << 60,
    AHARDWAREBUFFER_USAGE_VENDOR_17 = 1ULL << 61,
    AHARDWAREBUFFER_USAGE_VENDOR_18 = 1ULL << 62,
    AHARDWAREBUFFER_USAGE_VENDOR_19 = 1ULL << 63,
};

/**
 * Buffer description. Used for allocating new buffers and querying
 * parameters of existing ones.
 */
typedef struct AHardwareBuffer_Desc {
    uint32_t width;  ///< Width in pixels.
    uint32_t height; ///< Height in pixels.
    /**
     * Number of images in an image array. AHardwareBuffers with one
     * layer correspond to regular 2D textures. AHardwareBuffers with
     * more than layer correspond to texture arrays. If the layer count
     * is a multiple of 6 and the usage flag
     * AHARDWAREBUFFER_USAGE_GPU_CUBE_MAP is present, the buffer is
     * a cube map or a cube map array.
     */
    uint32_t layers;
    uint32_t format; ///< One of AHardwareBuffer_Format.
    uint64_t usage;  ///< Combination of AHardwareBuffer_UsageFlags.
    uint32_t stride; ///< Row stride in pixels, ignored for AHardwareBuffer_allocate()
    uint32_t rfu0;   ///< Initialize to zero, reserved for future use.
    uint64_t rfu1;   ///< Initialize to zero, reserved for future use.
} AHardwareBuffer_Desc;

/**
 * Holds data for a single image plane.
 */
typedef struct AHardwareBuffer_Plane {
    void*    data; ///< Points to first byte in plane
    uint32_t pixelStride;    ///< Distance in bytes from the color channel of one pixel to the next
    uint32_t rowStride;      ///< Distance in bytes from the first value of one row of the image to
                             ///  the first value of the next row.
} AHardwareBuffer_Plane;

/**
 * Holds all image planes that contain the pixel data.
 */
typedef struct AHardwareBuffer_Planes {
    uint32_t              planeCount; ///< Number of distinct planes
    AHardwareBuffer_Plane planes[4];  ///< Array of image planes
} AHardwareBuffer_Planes;

/**
 * Opaque handle for a native hardware buffer.
 */
typedef struct AHardwareBuffer AHardwareBuffer;


}
