// Bento includes
#include <bento_base/security.h>
#include <bento_memory/common.h>
#include <bento_collection/vector.h>


// Library includes
#include "bento_graphics/vulkan_backend.h"

#if defined(VULKAN_SUPPORTED) && defined(GLFW_SUPPORTED)
// External includes
#if defined(WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <vulkan/vulkan.h>
	#define GLFW_INCLUDE_VULKAN
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WGL
	#include <windows.h>
	#include <GLFW/glfw3.h>
	#include <GLFW/glfw3native.h>
#endif

namespace bento
{
	namespace vulkan
	{
		struct VKRenderEnvironment
		{
			// Platform speicifc window
			GLFWwindow* window;

			// Vulkan instance
			VkInstance instance;

			// Physical device (GPU)
			VkPhysicalDevice physical_device;

			// Logical device to interact with the physical device
			VkDevice logical_device;

			// Surface where vulkan renders
			VkSurfaceKHR surface;

			// Rendering queue data
			uint32_t renderingQueueIdx;
			VkQueue renderingQueue;

			// Preset queue data
			uint32_t presentQueueIdx;
			VkQueue presentQueue;

			// Allocator used for the structure
			bento::IAllocator* _allocator;
		};

		namespace render_system
		{
			bool init_render_system()
			{
				return glfwInit() && glfwVulkanSupported();
			}

			void shutdown_render_system()
			{
				glfwTerminate();
			}

			uint32_t first_compatible_device(VkPhysicalDevice* device_array, uint64_t num_devices)
			{
				for (uint32_t device_idx = 0; device_idx < num_devices; ++device_idx)
				{
					VkPhysicalDevice& current_device = device_array[device_idx];

					VkPhysicalDeviceProperties deviceProperties;
					VkPhysicalDeviceFeatures deviceFeatures;

					vkGetPhysicalDeviceProperties(current_device, &deviceProperties);
					vkGetPhysicalDeviceFeatures(current_device, &deviceFeatures);

					if (deviceFeatures.geometryShader && deviceFeatures.tessellationShader)
					{
						return device_idx;
					}
				}
				return UINT32_MAX;
			}

			uint32_t find_rendering_queue(VkQueueFamilyProperties* queue_array, uint64_t num_queues)
			{
				// Iterate through the queues
				for (uint32_t queue_idx = 0; queue_idx < num_queues; ++queue_idx)
				{
					// Get the current properties
					const VkQueueFamilyProperties& queueProperties = queue_array[queue_idx];
					if (queueProperties.queueCount == 0)
						continue;

					// Find a queue that has all the requirements
					if ((queueProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
						&& (queueProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)
						&& (queueProperties.queueFlags & VK_QUEUE_TRANSFER_BIT))
						return queue_idx;
				}
				return UINT32_MAX;
			}

			void find_present_queue(VKRenderEnvironment* vk_re, uint32_t num_queues)
			{
				// Loop through the quques
				for (uint32_t queue_idx = 0; queue_idx < num_queues; ++queue_idx)
				{
					// 
					VkBool32 present_support;
					if (vkGetPhysicalDeviceSurfaceSupportKHR(vk_re->physical_device, queue_idx, vk_re->surface, &present_support) == VK_SUCCESS && present_support)
					{
						vk_re->presentQueueIdx = queue_idx;
						return;
					}
				}
			}

			VkQueue retrieve_queue_by_index(VKRenderEnvironment* render_environement, uint32_t queue_index)
			{
				assert(render_environement);
				VkQueue graphicsQueue;
				vkGetDeviceQueue(render_environement->logical_device, queue_index, 0, &graphicsQueue);
				return graphicsQueue;
			}

			RenderEnvironment create_render_environment(uint32_t width, uint32_t height, const char* windowName, bento::IAllocator& allocator)
			{
				// Hint to glfw that we will be using vulkan
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

				// Create our global structure for the render environment
				VKRenderEnvironment* vk_re = bento::make_new<VKRenderEnvironment>(allocator);
				vk_re->_allocator = &allocator;
				vk_re->window = glfwCreateWindow(width, height, windowName, NULL, NULL);

				// Grab all the extensions
				unsigned int glfwExtensionCount = 0;
				const char ** glfwExtensions;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

				// Define our application info
				VkApplicationInfo app_info = {};
				app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				app_info.pApplicationName = windowName;
				app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				app_info.pEngineName = "bento";
				app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
				app_info.apiVersion = VK_API_VERSION_1_0;

				// Define our instance info
				VkInstanceCreateInfo create_info = {};
				create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				create_info.pApplicationInfo = &app_info;
				create_info.enabledExtensionCount = glfwExtensionCount;
				create_info.ppEnabledExtensionNames = glfwExtensions;
				create_info.enabledLayerCount = 0;

				// Create our vulkan instance
				assert_msg(vkCreateInstance(&create_info, nullptr, &vk_re->instance) == VK_SUCCESS, "Failed to create a vulkan instance");

				// Make sure that at least one device can be used
				uint32_t deviceCount = 0;
				vkEnumeratePhysicalDevices(vk_re->instance, &deviceCount, nullptr);
				assert_msg(deviceCount != 0, "No vulkan compatible devices");

				// Grab the list of devices
				bento::Vector<VkPhysicalDevice> devices(allocator, deviceCount);
				vkEnumeratePhysicalDevices(vk_re->instance, &deviceCount, devices.begin());

				// Evaluate the device we shall be using
				uint32_t best_device = first_compatible_device(devices.begin(), deviceCount);
				assert_msg(best_device != UINT32_MAX, "No compatible vulkan device");

				// Keep track of the "best" physical device 
				vk_re->physical_device = devices[(uint32_t)best_device];

				// Get the number of queue famiies
				uint32_t queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(vk_re->physical_device, &queueFamilyCount, nullptr);

				// List the queue family properties
				bento::Vector<VkQueueFamilyProperties> queueFamilies(allocator, queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(vk_re->physical_device, &queueFamilyCount, queueFamilies.begin());

				// Check for the queues
				vk_re->renderingQueueIdx = find_rendering_queue(queueFamilies.begin(), queueFamilyCount);
				assert_msg(vk_re->renderingQueueIdx != UINT32_MAX, "No valid rendering queue found");


				// Queue creation descriptor
				VkDeviceQueueCreateInfo queue_create_info = {};
				queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queue_create_info.queueFamilyIndex = vk_re->renderingQueueIdx;
				queue_create_info.queueCount = 1;
				float queue_priority = 1.0f;
				queue_create_info.pQueuePriorities = &queue_priority;

				// Descriptor for the logial device
				VkDeviceCreateInfo device_create_info = {};
				device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				device_create_info.pQueueCreateInfos = &queue_create_info;
				device_create_info.queueCreateInfoCount = 1;
				VkPhysicalDeviceFeatures deviceFeatures = {};
				device_create_info.pEnabledFeatures = &deviceFeatures;
				device_create_info.enabledExtensionCount = 0;
				device_create_info.enabledLayerCount = 0;

				// Create the logical device from the physical device
				assert_msg(vkCreateDevice(devices[best_device], &device_create_info, nullptr, &vk_re->logical_device) == VK_SUCCESS, "Logical device creation failed");

				// Retrieve the rendering queue
				vkGetDeviceQueue(vk_re->logical_device, vk_re->renderingQueueIdx, 0, &vk_re->renderingQueue);

				// Create the surface that vulkan will be rendering into (inside the of the window)
				assert_msg(glfwCreateWindowSurface(vk_re->instance, vk_re->window, nullptr, &(vk_re->surface)) == VK_SUCCESS, "Surface creation failed");
				
				// Queue for the present support
				find_present_queue(vk_re, queueFamilyCount);
				assert_msg(vk_re->presentQueueIdx != UINT32_MAX, "No valid present queue found");

				// Retrieve the present queue
				vkGetDeviceQueue(vk_re->logical_device, vk_re->presentQueueIdx, 0, &vk_re->presentQueue);

				return (RenderEnvironment)vk_re;
			}

			void destroy_render_environment(RenderEnvironment render_environment)
			{
				// Cast the pointer and make sure it is not null
				VKRenderEnvironment* vk_re = (VKRenderEnvironment*)render_environment;
				assert(vk_re != nullptr);

				// Destroy the vulkan structures
				vkDestroySurfaceKHR(vk_re->instance, vk_re->surface, nullptr);
				vkDestroyDevice(vk_re->logical_device, nullptr);
				vkDestroyInstance(vk_re->instance, nullptr);

				// Destroy the window
				glfwDestroyWindow(vk_re->window);

				// Destroy the internal structure
				bento::make_delete<VKRenderEnvironment>(*vk_re->_allocator, vk_re);
			}

			RenderWindow render_window(RenderEnvironment render_environement)
			{
				VKRenderEnvironment* vk_re = (VKRenderEnvironment*)render_environement;
				return (RenderWindow)vk_re->window;
			}

			void collect_inputs(RenderEnvironment)
			{
				glfwPollEvents();
			}

			float get_time(RenderEnvironment)
			{
				return (float)glfwGetTime();
			}
		}

		namespace window
		{
			void show(RenderWindow window)
			{
				GLFWwindow* window_ptr = (GLFWwindow*)window;
				glfwShowWindow(window_ptr);
			}

			void hide(RenderWindow window)
			{
				GLFWwindow* window_ptr = (GLFWwindow*)window;
				glfwHideWindow(window_ptr);
			}

			bool is_active(RenderWindow window)
			{
				GLFWwindow* window_ptr = (GLFWwindow*)window;
				return !glfwWindowShouldClose(window_ptr);
			}

			void swap(RenderWindow window)
			{
				GLFWwindow* window_ptr = (GLFWwindow*)window;
				glfwSwapBuffers(window_ptr);
			}
		}

		namespace framebuffer
		{
			struct VK_FrameBuffer
			{
				VkFramebuffer vkFrameBuffer;
				VKRenderEnvironment* vkRenderEnvironement;
				bento::IAllocator* _allocator;
			};

			FramebufferObject create(RenderEnvironment renderEnviroment, bento::IAllocator& allocator)
			{
				// Cast the render environment and make sure it is not null
				VKRenderEnvironment* vkRenderEnvironement = (VKRenderEnvironment*)renderEnviroment;
				assert(vkRenderEnvironement != nullptr);
				
				// Allocate the structure
				VK_FrameBuffer* vkFrameBuffer = bento::make_new<VK_FrameBuffer>(allocator);
				vkFrameBuffer->vkRenderEnvironement = vkRenderEnvironement;
				vkFrameBuffer->_allocator = &allocator;

				// Create the vulkan frame buffer
				VkFramebufferCreateInfo frameBufferInfo;
				frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				frameBufferInfo.renderPass = nullptr;
				frameBufferInfo.attachmentCount = 1;
				frameBufferInfo.pAttachments = nullptr;
				frameBufferInfo.width = 1;
				frameBufferInfo.height = 1;
				frameBufferInfo.layers = 1;
				vkCreateFramebuffer(vkRenderEnvironement->logical_device, &frameBufferInfo, NULL, &vkFrameBuffer->vkFrameBuffer);

				return (FramebufferObject)vkFrameBuffer;
			}

			void destroy(FramebufferObject frame_buffer)
			{
				// Cast and make sure it is not null
				VK_FrameBuffer* vkFrameBuffer = (VK_FrameBuffer*)frame_buffer;
				assert(vkFrameBuffer != nullptr);

				// Destroy the vulkan frame buffer
				vkDestroyFramebuffer(vkFrameBuffer->vkRenderEnvironement->logical_device, vkFrameBuffer->vkFrameBuffer, nullptr);

				// Destroy our structure
				bento::make_delete<VK_FrameBuffer>(*vkFrameBuffer->_allocator, vkFrameBuffer);
			}

			bool check(FramebufferObject)
			{
				return false;
			}

			void bind(FramebufferObject)
			{
			}

			void unbind(FramebufferObject)
			{
			}

			void clear(FramebufferObject)
			{
			}

			void set_clear_color(FramebufferObject, const bento::Vector4&)
			{
			}

			void bind_texture(FramebufferObject, const TextureObject&)
			{
			}

			void set_num_render_targets(FramebufferObject, uint8_t)
			{
			}

			void enable_depth_test(FramebufferObject)
			{
			}

			void disable_depth_test(FramebufferObject)
			{
			}
		}

		namespace texture
		{
			TextureObject create()
			{
				return 0;
			}

			void destroy(TextureObject)
			{
			}

			void set_debug_name(TextureObject, const char*)
			{
			}
		}
		namespace render_section
		{
			void start_render_section(const char*)
			{
			}

			void end_render_section()
			{
			}
		}

		namespace geometry
		{
			GeometryObject create_vnt(const float*, uint32_t, const unsigned*, uint32_t)
			{
				return 0;
			}

			void destroy_vnt(GeometryObject)
			{
			}
		}

		namespace shader
		{
			ShaderPipelineObject create_shader_from_source(const char*, const char*,
				const char*, const char*,
				const char*)
			{
				return 0;
			}

			void destroy_shader(ShaderPipelineObject)
			{
			}

			void bind_shader(ShaderPipelineObject)
			{
			}

			void unbind_shader(ShaderPipelineObject)
			{
			}

			void inject_int(ShaderPipelineObject, int, const char*)
			{
			}

			void inject_float(ShaderPipelineObject, float, const char*)
			{
			}

			void inject_vec3(ShaderPipelineObject, const bento::Vector3&, const char*)
			{
			}

			void inject_vec4(ShaderPipelineObject, const bento::Vector4&, const char*)
			{
			}

			void inject_mat3(ShaderPipelineObject, const bento::Matrix3&, const char*)
			{
			}

			void inject_mat4(ShaderPipelineObject, const bento::Matrix4&, const char*)
			{
			}

			void inject_array(ShaderPipelineObject, const float*, uint32_t, const char*)
			{
			}

			void inject_texture(ShaderPipelineObject, TextureObject, uint32_t, const char*)
			{
			}
		}
	}
}

#endif // VULKAN_SUPPORTED