// Bento includes
#include <bento_base/security.h>
#include <bento_base/log.h>
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

// Other includes
#include <algorithm>

// To avoid collision with the macros
#undef max
#undef min

namespace bento
{
    namespace vulkan
    {
        class VK_RenderEnvironment_Internal
        {
        public:
            ALLOCATOR_BASED;
            VK_RenderEnvironment_Internal(bento::IAllocator& allocator)
            : _allocator(allocator)
            , window(nullptr)
            , renderingQueueIdx(UINT32_MAX)
            , presentQueueIdx(UINT32_MAX)
            , swapChainImages(allocator)
            , swapChainImageViews(allocator)
            {

            }

            // Platform specific window
            GLFWwindow* window;

            // VK instance
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

            // Swap chain data
            VkSwapchainKHR swapChain;
            VkFormat swapChainImageFormat;
            VkExtent2D swapChainExtent;
            bento::Vector<VkImage> swapChainImages;
            bento::Vector<VkImageView> swapChainImageViews;

            // Allocator used for the structure
            bento::IAllocator& _allocator;
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

            uint32_t first_compatible_device(const bento::Vector<VkPhysicalDevice>& deviceArray)
            {
                for (uint32_t device_idx = 0; device_idx < deviceArray.size(); ++device_idx)
                {
                    const VkPhysicalDevice& current_device = deviceArray[device_idx];

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

            void find_present_queue(VK_RenderEnvironment_Internal* vk_re, uint32_t num_queues)
            {
                // Loop through the queues
                for (uint32_t queue_idx = 0; queue_idx < num_queues; ++queue_idx)
                {
                    VkBool32 present_support;
                    if (vkGetPhysicalDeviceSurfaceSupportKHR(vk_re->physical_device, queue_idx, vk_re->surface, &present_support) == VK_SUCCESS && present_support)
                    {
                        vk_re->presentQueueIdx = queue_idx;
                        return;
                    }
                }
            }

            VkQueue retrieve_queue_by_index(VK_RenderEnvironment_Internal* render_environement, uint32_t queue_index)
            {
                assert(render_environement);
                VkQueue graphicsQueue;
                vkGetDeviceQueue(render_environement->logical_device, queue_index, 0, &graphicsQueue);
                return graphicsQueue;
            }

            void enumerate_physical_devices(VkInstance& vkInstance, bento::Vector<VkPhysicalDevice>& devices)
            {
                // Make sure that at least one device can be used
                uint32_t deviceCount = 0;
                vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
                assert_msg(deviceCount != 0, "No vulkan compatible devices");

                // Grab the list of devices
                devices.resize(deviceCount);
                vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.begin());
            }

            void enumerate_device_queue_family_properties(VkPhysicalDevice& vkPhysicalDevice, bento::Vector<VkQueueFamilyProperties>& queueFamilies)
            {
                // Get the number of queue families
                uint32_t queueFamilyCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

                if (queueFamilyCount != 0)
                {
                    // List the queue family properties
                    queueFamilies.resize(queueFamilyCount);
                    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.begin());
                }
            }

            void enumerate_physical_device_formats(VkPhysicalDevice& vkPhysicalDevice, VkSurfaceKHR vkSurface, bento::Vector<VkSurfaceFormatKHR>& formats)
            {
                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &formatCount, nullptr);

                if (formatCount != 0)
                {
                    formats.resize(formatCount);
                    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &formatCount, formats.begin());
                }
            }

            void enumerate_physical_device_present_modes(VkPhysicalDevice& vkPhysicalDevice, VkSurfaceKHR vkSurface, bento::Vector<VkPresentModeKHR>& present_modes)
            {
                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, nullptr);

                if (presentModeCount != 0)
                {
                    present_modes.resize(presentModeCount);
                    vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurface, &presentModeCount, present_modes.begin());
                }
            }

            VkExtent2D evaluate_swap_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
            {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                };
                actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(actualExtent.width, capabilities.maxImageExtent.width));
                actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(actualExtent.height, capabilities.maxImageExtent.height));
                return actualExtent;
            }

            void create_swap_chain(VK_RenderEnvironment_Internal* vk_re, IAllocator& allocator)
            {
                // Get the capabilities of the physical device
                VkSurfaceCapabilitiesKHR capabilities;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_re->physical_device, vk_re->surface, &capabilities);
                VkExtent2D extent = evaluate_swap_extent(vk_re->window, capabilities);

                // Enumerate all the formats
                bento::Vector<VkSurfaceFormatKHR> formats(allocator);
                enumerate_physical_device_formats(vk_re->physical_device, vk_re->surface, formats);
                assert_msg(formats.size() != 0, "No device formats available.");

                // Loop through the formats and look for one that fits our needs
                uint32_t targetFormat = UINT32_MAX;
                for (uint32_t formatIdx = 0; formatIdx < formats.size(); ++formatIdx)
                {
                    // Grab the current format
                    const VkSurfaceFormatKHR& currentFormat = formats[formatIdx];
                    if (currentFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                        && currentFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    {
                        targetFormat = formatIdx;
                        break;
                    }
                }
                assert_msg(targetFormat != UINT32_MAX, "No valid format found.");

                // Enumerate all the present modes
                bento::Vector<VkPresentModeKHR> presentModes(allocator);
                enumerate_physical_device_present_modes(vk_re->physical_device, vk_re->surface, presentModes);
                assert_msg(presentModes.size() != 0, "No present modes available.");

                // Look for the present mode we need
                uint32_t targetMode = UINT32_MAX;
                for (uint32_t modeIdx = 0; modeIdx < presentModes.size(); ++modeIdx)
                {
                    // Grab the current format
                    const VkPresentModeKHR& currentMode = presentModes[modeIdx];
                    if (currentMode == VK_PRESENT_MODE_FIFO_KHR)
                    {
                        targetMode = modeIdx;
                        break;
                    }
                }
                assert_msg(targetMode != UINT32_MAX, "No valid present mode found.");

                VkSwapchainCreateInfoKHR createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                createInfo.surface = vk_re->surface;
                createInfo.minImageCount = capabilities.minImageCount + 1;
                createInfo.imageFormat = formats[targetFormat].format;
                createInfo.imageColorSpace = formats[targetFormat].colorSpace;
                createInfo.imageExtent = extent;
                createInfo.imageArrayLayers = 1;
                createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

                if (vk_re->renderingQueueIdx != vk_re->presentQueueIdx)
                {
                    uint32_t queueFamilyIndices[] = { vk_re->renderingQueueIdx, vk_re->presentQueueIdx };
                    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                    createInfo.queueFamilyIndexCount = 2;
                    createInfo.pQueueFamilyIndices = queueFamilyIndices;
                }
                else
                {
                    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    createInfo.queueFamilyIndexCount = 0; // Optional
                    createInfo.pQueueFamilyIndices = nullptr; // Optional
                }
                createInfo.preTransform = capabilities.currentTransform;
                createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                createInfo.presentMode = presentModes[targetMode];
                createInfo.clipped = VK_TRUE;
                createInfo.oldSwapchain = VK_NULL_HANDLE;
                assert_msg(vkCreateSwapchainKHR(vk_re->logical_device, &createInfo, nullptr, &vk_re->swapChain) == VK_SUCCESS, "Failed to create swap chain.");

                // Grab all the swap chain images
                uint32_t imageCount;
                vkGetSwapchainImagesKHR(vk_re->logical_device, vk_re->swapChain, &imageCount, nullptr);
                vk_re->swapChainImages.resize(imageCount);
                vkGetSwapchainImagesKHR(vk_re->logical_device, vk_re->swapChain, &imageCount, vk_re->swapChainImages.begin());

                // Keep track of the additional data
                vk_re->swapChainImageFormat = formats[targetFormat].format;
                vk_re->swapChainExtent = extent;

                // Allocate for the image view
                vk_re->swapChainImageViews.resize(imageCount);

                // Create the image views
                for (uint32_t i = 0; i < imageCount; i++)
                {
                    VkImageViewCreateInfo createInfoIV{};
                    createInfoIV.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    createInfoIV.image = vk_re->swapChainImages[i];
                    createInfoIV.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    createInfoIV.format = vk_re->swapChainImageFormat;
                    createInfoIV.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfoIV.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfoIV.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfoIV.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfoIV.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    createInfoIV.subresourceRange.baseMipLevel = 0;
                    createInfoIV.subresourceRange.levelCount = 1;
                    createInfoIV.subresourceRange.baseArrayLayer = 0;
                    createInfoIV.subresourceRange.layerCount = 1;

                    // Create the actual image view
                    assert_msg(vkCreateImageView(vk_re->logical_device, &createInfoIV, nullptr, &vk_re->swapChainImageViews[i]) == VK_SUCCESS, "Failed to create image views.");
                }
            }

            void pick_physical_device(VK_RenderEnvironment_Internal* vk_re, IAllocator& allocator)
            {
                // Grab the list of physical devices
                bento::Vector<VkPhysicalDevice> devices(allocator);
                enumerate_physical_devices(vk_re->instance, devices);

                // Evaluate which device we shall be using
                uint32_t best_device = first_compatible_device(devices);
                assert_msg(best_device != UINT32_MAX, "No compatible vulkan device");

                // Keep track of the "best" physical device 
                vk_re->physical_device = devices[(uint32_t)best_device];
            }

            void create_logical_device(VK_RenderEnvironment_Internal* vk_re, IAllocator& allocator)
            {
                // Queue creation descriptor
                VkDeviceQueueCreateInfo queue_create_info = {};
                queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_create_info.queueFamilyIndex = vk_re->renderingQueueIdx;
                queue_create_info.queueCount = 1;
                float queue_priority = 1.0f;
                queue_create_info.pQueuePriorities = &queue_priority;

                // Allocator the vector that holds all the additional extensions that are required
                bento::Vector<const char*> requiredExtensions(allocator, 1);
                // The swap chain extension
                requiredExtensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

                // Descriptor for the logical device
                VkDeviceCreateInfo device_create_info = {};
                device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                device_create_info.pQueueCreateInfos = &queue_create_info;
                device_create_info.queueCreateInfoCount = 1;

                VkPhysicalDeviceFeatures deviceFeatures = {};
                device_create_info.pEnabledFeatures = &deviceFeatures;
                device_create_info.enabledExtensionCount = requiredExtensions.size();
                device_create_info.ppEnabledExtensionNames = requiredExtensions.begin();
                device_create_info.enabledLayerCount = 0;

                // Create the logical device from the physical device
                assert_msg(vkCreateDevice(vk_re->physical_device, &device_create_info, nullptr, &vk_re->logical_device) == VK_SUCCESS, "Logical device creation failed");
            }

            void create_vulkan_instance(VK_RenderEnvironment_Internal* vk_re, const char* window_name)
            {
                // Define our application info
                VkApplicationInfo app_info = {};
                app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                app_info.pApplicationName = window_name;
                app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
                app_info.pEngineName = "bento";
                app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
                app_info.apiVersion = VK_API_VERSION_1_0;

                // Grab all the extensions that GLFW requires
                unsigned int glfwExtensionCount = 0;
                const char ** glfwExtensions;
                glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

                // Define our instance info
                VkInstanceCreateInfo create_info = {};
                create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                create_info.pApplicationInfo = &app_info;
                create_info.enabledExtensionCount = glfwExtensionCount;
                create_info.ppEnabledExtensionNames = glfwExtensions;
                create_info.enabledLayerCount = 0;

                // Create our vulkan instance
                assert_msg(vkCreateInstance(&create_info, nullptr, &vk_re->instance) == VK_SUCCESS, "Failed to create a vulkan instance.");
            }

            RenderEnvironment create_render_environment(uint32_t width, uint32_t height, const char* windowName, bento::IAllocator& allocator)
            {
                // Hint to GLFW that we will be using vulkan
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

                // Create our global structure for the render environment
                VK_RenderEnvironment_Internal* vk_re = bento::make_new<VK_RenderEnvironment_Internal>(allocator, allocator);
                vk_re->window = glfwCreateWindow(width, height, windowName, NULL, NULL);

                // Create the vulkan instance
                create_vulkan_instance(vk_re, windowName);

                // Pick a physical device
                pick_physical_device(vk_re, allocator);

                // Enumerate the queue family properties
                bento::Vector<VkQueueFamilyProperties> queueFamilies(allocator);
                enumerate_device_queue_family_properties(vk_re->physical_device, queueFamilies);

                // Find the rendering queue (graphics, compute and transfer)
                vk_re->renderingQueueIdx = find_rendering_queue(queueFamilies.begin(), queueFamilies.size());
                assert_msg(vk_re->renderingQueueIdx != UINT32_MAX, "No valid rendering queue found");

                // Create the logical device
                create_logical_device(vk_re, allocator);

                // Retrieve the rendering queue
                vkGetDeviceQueue(vk_re->logical_device, vk_re->renderingQueueIdx, 0, &vk_re->renderingQueue);

                // Create the surface that vulkan will be rendering into (inside the of the window)
                assert_msg(glfwCreateWindowSurface(vk_re->instance, vk_re->window, nullptr, &(vk_re->surface)) == VK_SUCCESS, "Surface creation failed");
                
                // Queue for the present support
                find_present_queue(vk_re, queueFamilies.size());
                assert_msg(vk_re->presentQueueIdx != UINT32_MAX, "No valid present queue found");

                // Retrieve the present queue
                vkGetDeviceQueue(vk_re->logical_device, vk_re->presentQueueIdx, 0, &vk_re->presentQueue);

                // Create the swap chain
                create_swap_chain(vk_re, allocator);

                // Opaque cast and return our internal structure
                return (RenderEnvironment)vk_re;
            }

            void destroy_render_environment(RenderEnvironment render_environment)
            {
                // Cast the pointer and make sure it is not null
                VK_RenderEnvironment_Internal* vk_re = (VK_RenderEnvironment_Internal*)render_environment;
                assert(vk_re != nullptr);

                // Destroy the vulkan structures
                uint32_t imageViewCount = vk_re->swapChainImageViews.size();
                for (uint32_t imageIdx = 0; imageIdx < imageViewCount; ++imageIdx)
                    vkDestroyImageView(vk_re->logical_device, vk_re->swapChainImageViews[imageIdx], nullptr);
                vkDestroySwapchainKHR(vk_re->logical_device, vk_re->swapChain, nullptr);
                vkDestroySurfaceKHR(vk_re->instance, vk_re->surface, nullptr);
                vkDestroyDevice(vk_re->logical_device, nullptr);
                vkDestroyInstance(vk_re->instance, nullptr);

                // Destroy the window
                glfwDestroyWindow(vk_re->window);

                // Destroy the internal structure
                bento::make_delete<VK_RenderEnvironment_Internal>(vk_re->_allocator, vk_re);
            }

            RenderWindow render_window(RenderEnvironment render_environement)
            {
                VK_RenderEnvironment_Internal* vk_re = (VK_RenderEnvironment_Internal*)render_environement;
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

        namespace command_buffer
        {
            struct VK_CommandBuffer_Internal
            {
                // Current render environment
                VK_RenderEnvironment_Internal* renderEnvironement;

                // Target command pool used to allocate the command buffer
                VkCommandPool commandPool;

                // Target command buffer
                VkCommandBuffer commandBuffer;

                // Allocator used for the structure
                bento::IAllocator* _allocator;
            };

            CommandBuffer create(RenderEnvironment render_environment, IAllocator& allocator)
            {
                // Cast the render environment and make sure it is not null
                VK_RenderEnvironment_Internal* vkRenderEnvironment = (VK_RenderEnvironment_Internal*)render_environment;
                assert(vkRenderEnvironment != nullptr);

                // Create the internal structure of the command buffer
                VK_CommandBuffer_Internal* vk_commandBuffer = bento::make_new<VK_CommandBuffer_Internal>(allocator);
                vk_commandBuffer->renderEnvironement = vkRenderEnvironment;
                vk_commandBuffer->_allocator = &allocator;

                // Create the command pool
                VkCommandPoolCreateInfo poolInfo{};
                poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                poolInfo.queueFamilyIndex = vkRenderEnvironment->renderingQueueIdx;
                poolInfo.flags = 0; // Optional
                assert_msg(vkCreateCommandPool(vkRenderEnvironment->logical_device, &poolInfo, nullptr, &vk_commandBuffer->commandPool) == VK_SUCCESS, "Failed to create command pool.");

                // Allocate the command buffer
                VkCommandBufferAllocateInfo allocInfo{};
                allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.commandPool = vk_commandBuffer->commandPool;
                allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocInfo.commandBufferCount = 1;
                assert_msg(vkAllocateCommandBuffers(vkRenderEnvironment->logical_device, &allocInfo, &vk_commandBuffer->commandBuffer) == VK_SUCCESS, "Failed to allocate the command buffers.");

                // Cast and return the command buffer
                return (CommandBuffer)vk_commandBuffer;
            }

            void destroy(CommandBuffer command_buffer)
            {
                // Cast our internal structure
                VK_CommandBuffer_Internal* vk_commandBuffer = (VK_CommandBuffer_Internal*)command_buffer;
                assert(vk_commandBuffer != nullptr);

                // Destroy the command pool
                vkDestroyCommandPool(vk_commandBuffer->renderEnvironement->logical_device, vk_commandBuffer->commandPool, nullptr);

                // Destroy the internal structure
                bento::make_delete<VK_CommandBuffer_Internal>(*vk_commandBuffer->_allocator, vk_commandBuffer);
            }

            void start_record(CommandBuffer command_buffer)
            {
                // Cast our internal structure
                VK_CommandBuffer_Internal* vk_commandBuffer = (VK_CommandBuffer_Internal*)command_buffer;
                assert(vk_commandBuffer != nullptr);

                // Start recording the command buffer
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = 0; // Optional
                beginInfo.pInheritanceInfo = nullptr; // Optional
                assert_msg(vkBeginCommandBuffer(vk_commandBuffer->commandBuffer, &beginInfo) == VK_SUCCESS, "failed to begin recording command buffer.");
            }

            void stop_record(CommandBuffer command_buffer)
            {
                // Cast our internal structure
                VK_CommandBuffer_Internal* vk_commandBuffer = (VK_CommandBuffer_Internal*)command_buffer;
                assert(vk_commandBuffer != nullptr);

                // End recording the command buffer
                assert_msg(vkEndCommandBuffer(vk_commandBuffer->commandBuffer) == VK_SUCCESS, "Failed to stop recording the command buffer.");
            }
        }

        namespace framebuffer
        {
            struct VK_FrameBuffer_Internal
            {
                VkFramebuffer vkFrameBuffer;
                VK_RenderEnvironment_Internal* vkRenderEnvironement;
                bento::IAllocator* _allocator;
            };

            FrameBufferObject create(RenderEnvironment render_environment, bento::IAllocator& allocator)
            {
                // Cast the render environment and make sure it is not null
                VK_RenderEnvironment_Internal* vkRenderEnvironement = (VK_RenderEnvironment_Internal*)render_environment;
                assert(vkRenderEnvironement != nullptr);
                
                // Allocate the structure
                VK_FrameBuffer_Internal* vkFrameBuffer = bento::make_new<VK_FrameBuffer_Internal>(allocator);
                vkFrameBuffer->vkRenderEnvironement = vkRenderEnvironement;
                vkFrameBuffer->_allocator = &allocator;

                // Create the vulkan frame buffer
                VkFramebufferCreateInfo frameBufferInfo;
                frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                frameBufferInfo.renderPass = nullptr;
                frameBufferInfo.attachmentCount = 0;
                frameBufferInfo.pAttachments = nullptr;
                frameBufferInfo.width = 1;
                frameBufferInfo.height = 1;
                frameBufferInfo.layers = 1;
                vkCreateFramebuffer(vkRenderEnvironement->logical_device, &frameBufferInfo, NULL, &vkFrameBuffer->vkFrameBuffer);

                return (FrameBufferObject)vkFrameBuffer;
            }

            void destroy(FrameBufferObject frame_buffer)
            {
                // Cast and make sure it is not null
                VK_FrameBuffer_Internal* vkFrameBuffer = (VK_FrameBuffer_Internal*)frame_buffer;
                assert(vkFrameBuffer != nullptr);

                // Destroy the vulkan frame buffer
                vkDestroyFramebuffer(vkFrameBuffer->vkRenderEnvironement->logical_device, vkFrameBuffer->vkFrameBuffer, nullptr);

                // Destroy our structure
                bento::make_delete<VK_FrameBuffer_Internal>(*vkFrameBuffer->_allocator, vkFrameBuffer);
            }
        }

        namespace texture
        {
            struct VK_Image_Internal
            {
                VkImage image;
                bento::texture::TextureDescriptor textureDescriptor;
                VK_RenderEnvironment_Internal* vkRenderEnvironement;
                bento::IAllocator* _allocator;
            };

            VkImageType GenericToVulkanTextureType(bento::texture::TextureType textureType)
            {
                switch (textureType)
                {
                    case bento::texture::Tex1D:
                        return VK_IMAGE_TYPE_1D;
                    case bento::texture::Tex2D:
                        return VK_IMAGE_TYPE_2D;
                    case bento::texture::Tex3D:
                        return VK_IMAGE_TYPE_3D;
                }
                return VK_IMAGE_TYPE_2D;
            }

            VkFormat GenericToVulkanTextureFormat(bento::texture::TextureFormat textureFormat)
            {
                switch (textureFormat)
                {
                    // R8
                    case bento::texture::R8_SNorm:
                        return VK_FORMAT_R8_SNORM;
                    case bento::texture::R8_UNorm:
                        return VK_FORMAT_R8_UNORM;
                    case bento::texture::R8_SInt:
                        return VK_FORMAT_R8_SINT;
                    case bento::texture::R8_UInt:
                        return VK_FORMAT_R8_UINT;

                    // R8G8
                    case bento::texture::R8G8_SNorm:
                        return VK_FORMAT_R8G8_SNORM;
                    case bento::texture::R8G8_UNorm:
                        return VK_FORMAT_R8G8_UNORM;
                    case bento::texture::R8G8_SInt:
                        return VK_FORMAT_R8G8_SINT;
                    case bento::texture::R8G8_UInt:
                        return VK_FORMAT_R8G8_UINT;

                    // R8G8B8A8
                    case bento::texture::R8G8B8A8_SNorm:
                        return VK_FORMAT_R8G8B8A8_SNORM;
                    case bento::texture::R8G8B8A8_UNorm:
                        return VK_FORMAT_R8G8B8A8_UNORM;
                    case bento::texture::R8G8B8A8_SInt:
                        return VK_FORMAT_R8G8B8A8_SINT;
                    case bento::texture::R8G8B8A8_UInt:
                        return VK_FORMAT_R8G8B8A8_UINT;

                    // R16
                    case bento::texture::R16_Float:
                        return VK_FORMAT_R16_SFLOAT;
                    case bento::texture::R16_SInt:
                        return VK_FORMAT_R16_SINT;
                    case bento::texture::R16_UInt:
                        return VK_FORMAT_R16_UINT;

                    // R16G16
                    case bento::texture::R16G16_Float:
                        return VK_FORMAT_R16G16_SFLOAT;
                    case bento::texture::R16G16_SInt:
                        return VK_FORMAT_R16G16_SINT;
                    case bento::texture::R16G16_UInt:
                        return VK_FORMAT_R16G16_UINT;

                    // R16G16B16A16
                    case bento::texture::R16G16B16A16_Float:
                        return VK_FORMAT_R16G16B16A16_SFLOAT;
                    case bento::texture::R16G16B16A16_SInt:
                        return VK_FORMAT_R16G16B16A16_SINT;
                    case bento::texture::R16G16B16A16_UInt:
                        return VK_FORMAT_R16G16B16A16_UINT;
                }
                return VK_FORMAT_R16G16B16A16_SFLOAT;
            }

            TextureObject create_texture(RenderEnvironment renderEnv, const bento::texture::TextureDescriptor& textureDescriptor, bento::IAllocator& allocator)
            {
                // Cast the render environment and make sure it is not null
                VK_RenderEnvironment_Internal* vkRenderEnvironement = (VK_RenderEnvironment_Internal*)renderEnv;
                assert(vkRenderEnvironement != nullptr);
                
                // Allocate the structure
                VK_Image_Internal* vkImage = bento::make_new<VK_Image_Internal>(allocator);
                vkImage->vkRenderEnvironement = vkRenderEnvironement;
                vkImage->textureDescriptor = textureDescriptor;
                vkImage->_allocator = &allocator;

                // Fill the creation info
                VkImageCreateInfo imageInfo = {};
                imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageInfo.imageType = GenericToVulkanTextureType(textureDescriptor.type);
                imageInfo.extent.width = textureDescriptor.width;
                imageInfo.extent.height = textureDescriptor.height;
                imageInfo.extent.depth = textureDescriptor.depth;
                imageInfo.mipLevels = textureDescriptor.hasMips ? (uint32_t)log2(std::max(textureDescriptor.width, textureDescriptor.height)) : 1;
                imageInfo.arrayLayers = 1;
                imageInfo.format = GenericToVulkanTextureFormat(textureDescriptor.format);
                imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
                imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                assert_msg(vkCreateImage(vkImage->vkRenderEnvironement->logical_device, &imageInfo, nullptr, &vkImage->image) == VK_SUCCESS,"Failed to create image.");
                
                // return the opaque structure
                return (TextureObject)vkImage;
            }

            void destroy_texture(TextureObject texture)
            {
                // Cast and make sure it is not null
                VK_Image_Internal* vkImage = (VK_Image_Internal*)texture;
                assert(vkImage != nullptr);

                // Destroy the vulkan image
                vkDestroyImage(vkImage->vkRenderEnvironement->logical_device, vkImage->image, nullptr);

                // Destroy our structure
                bento::make_delete<VK_Image_Internal>(*vkImage->_allocator, vkImage);
            }
        }

		namespace render_pass
		{
			struct VK_RenderPass_Internal
			{
				ALLOCATOR_BASED;
				VK_RenderPass_Internal(bento::IAllocator& allocator)
				: renderPass(nullptr)
				, vkRenderEnvironement(nullptr)
				, attachements(allocator)
				, attachementsRef(allocator)
				, _allocator(&allocator)
				{
				}

				// Internal vulkan render pass
				VkRenderPass renderPass;

				// Color attachments
				Vector<VkAttachmentDescription> attachements;
				Vector<VkAttachmentReference> attachementsRef;

				// Internal vulkan environment data
				VK_RenderEnvironment_Internal* vkRenderEnvironement;

				// Allocator used for this 
				bento::IAllocator* _allocator;
			};

			RenderPassObject create(RenderEnvironment renderEnvironment, bento::DrawDescriptor& drawDescriptor, bento::IAllocator& allocator)
			{
				// Cast the render environment and make sure it is not null
				VK_RenderEnvironment_Internal* vkRenderEnvironement = (VK_RenderEnvironment_Internal*)renderEnvironment;
				assert(vkRenderEnvironement != nullptr);

				// Allocate the structure
				VK_RenderPass_Internal* vkRenderPass = bento::make_new<VK_RenderPass_Internal>(allocator, allocator);
				vkRenderPass->vkRenderEnvironement = vkRenderEnvironement;

				// We need to declare all our attachements
				uint32_t numTargets = drawDescriptor.renderTargets.size();
				vkRenderPass->attachements.resize(numTargets);

				// Let's process all the "color" targets
				for (uint32_t attachIdx = 0; attachIdx < numTargets; ++attachIdx)
				{
					// Grab the current descriptor to fill
					VkAttachmentDescription& currentAttach = vkRenderPass->attachements[attachIdx];
					VkAttachmentReference& currentAttachRef = vkRenderPass->attachementsRef[attachIdx];

					// Grab the image we need to use
					texture::VK_Image_Internal* vkImage = (texture::VK_Image_Internal*)(drawDescriptor.renderTargets[attachIdx]);

					// Fill the attachment data
					currentAttach.format = texture::GenericToVulkanTextureFormat(vkImage->textureDescriptor.format);
					currentAttach.samples = VK_SAMPLE_COUNT_1_BIT;
					currentAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					currentAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					currentAttach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					currentAttach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					currentAttach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					currentAttach.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					// Attachment references
					currentAttachRef.attachment = attachIdx;
					currentAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				// Create our subpass
				VkSubpassDescription subpass{};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = numTargets;
				subpass.pColorAttachments = vkRenderPass->attachementsRef.begin();

				// Create the vulkan object
				VkRenderPassCreateInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = numTargets;
				renderPassInfo.pAttachments = vkRenderPass->attachements.begin();
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpass;
				assert_msg(vkCreateRenderPass(vkRenderEnvironement->logical_device, &renderPassInfo, nullptr, &vkRenderPass->renderPass) == VK_SUCCESS, "Failed to create render pass!");

				// Convert to opaque type
				return (RenderPassObject)vkRenderPass;
			}

			void destroy(RenderPassObject render_pass)
			{
				// Cast and make sure it is not null
				VK_RenderPass_Internal* vkRenderPass = (VK_RenderPass_Internal*)render_pass;
				assert(vkRenderPass != nullptr);

				// Destroy the render pass
				vkDestroyRenderPass(vkRenderPass->vkRenderEnvironement->logical_device, vkRenderPass->renderPass, nullptr);

				// Destroy the internal structure
				bento::make_delete<VK_RenderPass_Internal>(*vkRenderPass->_allocator, vkRenderPass);
			}
		}
    }
}

#endif // VULKAN_SUPPORTED