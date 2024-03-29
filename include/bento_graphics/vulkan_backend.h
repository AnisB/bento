#pragma once

// SDK includes
#include "bento_math/types.h"

#if defined(VULKAN_SUPPORTED) && defined(GLFW_SUPPORTED)
namespace bento
{
	// Types definition
	typedef uint64_t RenderEnvironment;
	typedef uint64_t RenderWindow;
	typedef uint64_t GeometryObject;
	typedef uint64_t FramebufferObject;
	typedef uint64_t TextureObject;
	typedef uint64_t CubemapObject;
	typedef uint64_t ShaderPipelineObject;
	typedef uint64_t CommandBuffer;

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

		namespace frame_buffer
		{
			FramebufferObject create(RenderEnvironment renderEnvironment, bento::IAllocator& allocator);
			void destroy(FramebufferObject frame_buffer);

			bool check(FramebufferObject framebuffer);

			void bind(FramebufferObject framebuffer);
			void unbind(FramebufferObject framebuffer);

			void bind_texture(FramebufferObject framebuffer, const TextureObject texture);

			void enable_depth_test(FramebufferObject framebuffer);
			void disable_depth_test(FramebufferObject framebuffer);

			void clear(FramebufferObject frame_buffer);
			void set_clear_color(FramebufferObject frame_buffer, const bento::Vector4& _color);

			void set_num_render_targets(FramebufferObject frmabuffer, uint8_t num_render_targets);
		}

		namespace texture
		{
			// Create and destroy textures
			TextureObject create();
			void destroy(TextureObject tex);

			// Set the debug name of the texture
			void set_debug_name(TextureObject texture, const char* debug_name);
		}

		namespace geometry
		{
			GeometryObject create_vnt(const float* dataArray, uint32_t numVert, const unsigned* indexArray, uint32_t numFaces);
			void destroy_vnt(GeometryObject object);
		}

		namespace shader
		{
			ShaderPipelineObject create_shader_from_source(const char* vertex_shader_source, const char* tess_control_shader_source,
				const char* tess_eval_shader_source, const char* geometry_shader_source,
				const char* fragment_shader_source);
			void destroy_shader(ShaderPipelineObject program);

			void bind_shader(ShaderPipelineObject program);
			void unbind_shader(ShaderPipelineObject program);

			void inject_int(ShaderPipelineObject program, int value, const char* slot);
			void inject_float(ShaderPipelineObject program, float value, const char* slot);
			void inject_vec3(ShaderPipelineObject program, const bento::Vector3& value, const char* slot);
			void inject_vec4(ShaderPipelineObject program, const bento::Vector4& value, const char* slot);
			void inject_mat3(ShaderPipelineObject program, const bento::Matrix3& value, const char* slot);
			void inject_mat4(ShaderPipelineObject program, const bento::Matrix4& value, const char* slot);

			void inject_array(ShaderPipelineObject program, const float* value_array, uint32_t numValues, const char* slot);

			void inject_texture(ShaderPipelineObject program, TextureObject texture_object, uint32_t offset, const char* slot);
			void inject_cubemap(ShaderPipelineObject program, CubemapObject cubemap_object, uint32_t offset, const char* slot);
		}

		namespace render_section
		{
			void start_render_section(const char* sectionName);
			void end_render_section();
		}
	}
}
#endif // VULKAN_BACKEND_H