#pragma once

// SDK includes
#include "bento_math/types.h"
#include "bento_graphics/common.h"

#if defined(VULKAN_SUPPORTED) && defined(GLFW_SUPPORTED)
namespace bento
{
    namespace vulkan
    {
        namespace render_system
        {
            // Render system
            bool init_render_system();
            void shutdown_render_system();

            // Render environment
            RenderEnvironment create_render_environment(uint32_t width, uint32_t height, const char* windowName, bento::IAllocator& allocator);
            void destroy_render_environment(RenderEnvironment render_environment);

            // Returns the native window
            RenderWindow render_window(RenderEnvironment render_environement);

            // Collects the set of inputs
            void collect_inputs(RenderEnvironment render_environement);

            // Return the global time of the application
            float get_time(RenderEnvironment render_environement);
        }

        namespace window
        {
            void show(RenderWindow window);
            void hide(RenderWindow window);
            bool is_active(RenderWindow window);
            void swap(RenderWindow window);
        }

        namespace command_buffer
        {
            CommandBuffer create(RenderEnvironment render_environment, IAllocator& allocator);
            void destroy(CommandBuffer command_buffer);
            void start_record(CommandBuffer command_buffer);
            void stop_record(CommandBuffer command_buffer);
        }

        namespace render_pass
        {
            RenderPassObject create(RenderEnvironment renderEnvironment, bento::DrawDescriptor& drawDescriptor, bento::IAllocator& allocator);
            void destroy(RenderPassObject render_pass);
        }

        namespace frame_buffer
        {
            FrameBufferObject create(RenderEnvironment renderEnvironment, bento::IAllocator& allocator);
            void destroy(FrameBufferObject frame_buffer);
        }

        namespace texture
        {
            TextureObject create_texture(RenderEnvironment renderEnv, const bento::texture::TextureDescriptor& textureDescriptor, bento::IAllocator& allocator);
            void destroy_texture(TextureObject texture);
        }
    }
}
#endif // VULKAN_BACKEND_H