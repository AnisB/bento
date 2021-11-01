#pragma once

// SDK includes
#include "bento_math/types.h"

namespace bento
{
    // Generic structures
    typedef uint64_t RenderEnvironment;
    typedef uint64_t RenderWindow;

    // Command buffer    
    typedef uint64_t CommandBuffer;

    // Rneder pass
    typedef uint64_t RenderPassObject;

    // Frame buffer
    typedef uint64_t FrameBufferObject;

    // Texture and UAV
    typedef uint64_t TextureObject;
    typedef uint64_t BufferObject;

    namespace render_pass
    {
        struct RenderPassDescriptor
        {
            ALLOCATOR_BASED;
            RenderPassDescriptor(bento::IAllocator& allocator)
            : textureArray(allocator)
            , bufferArray(allocator)
            , _allocator(allocator)
            {
            }

            Vector<TextureObject> textureArray;
            Vector<BufferObject> bufferArray;
            IAllocator& _allocator;
        };
    }

    namespace texture
    {
        enum TextureFormat
        {
            R8_SNorm,
            R8_UNorm,
            R8_SInt,
            R8_UInt,

            R8G8_SNorm,
            R8G8_UNorm,
            R8G8_SInt,
            R8G8_UInt,

            R8G8B8A8_SNorm,
            R8G8B8A8_UNorm,
            R8G8B8A8_SInt,
            R8G8B8A8_UInt,
            
            R16_Float,
            R16_SInt,
            R16_UInt,

            R16G16_Float,
            R16G16_SInt,
            R16G16_UInt,

            R16G16B16A16_Float,
            R16G16B16A16_SInt,
            R16G16B16A16_UInt,
        };

        enum TextureType
        {
            Tex1D,
            Tex2D,
            Tex3D
        };

        struct TextureDescriptor
        {
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            bool randomReadWrite;
            bool hasMips;
            TextureType type;
            TextureFormat format;
        };
    }
}