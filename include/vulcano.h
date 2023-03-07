#pragma once

#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <ui/termcolour.h>
#include <stdbool.h>
#include <stdint.h>
#include <cglm/cglm.h>

typedef struct {
    // SDL Window
    SDL_Window *vulcano_window;

    // Vulkan Instance
    VkInstance instance;

    // Vulkan Instance Extensions Count
    uint32_t vulkan_instance_extensions_count;

    // Vulkan Instance Extensions Name
    VkExtensionProperties *vulkan_instance_extensions;

    // Vulkan Layer Extensions Count
    uint32_t vulkan_layer_extensions_count;

    // Vulkan Layer Extensions Name
    VkLayerProperties *vulkan_layer_extensions;

    // Vulkan Device Extensions Count
    uint32_t vulkan_device_extensions_count;

    // Vulkan Device Extensions Name
    VkExtensionProperties *vulkan_device_extensions;

    uint32_t sdl_instance_extension_count;

    const char** sdl_instance_extensions;

    // Vulkan Physical Devices
    VkPhysicalDevice *physical_devices;

    // Chosen Vulkan Physical Device
    VkPhysicalDevice *phys_dev;

    // Vulkan Queue Family Properties from a Vulkan's Physical Device
    VkQueueFamilyProperties *queue_family_props;

    // Vulkan's Primary Device
    VkDevice device;
    
    size_t vk_queue_family_idx;
    size_t vk_queue_mode;

    // Vulkan Surface
    VkSurfaceKHR surface;

    // Vulkan Surface Capabilities
    VkSurfaceCapabilitiesKHR vk_surface_capabilities;

    VkQueue draw_queue;
    VkQueue present_queue;

    // Vulkan Surface Format
    VkSurfaceFormatKHR vk_surface_format;
    VkPresentModeKHR vk_surface_chosen_present;

    // Vulkan's Swapchain
    VkSwapchainKHR vk_swapchain;
	VkExtent2D vk_swapchain_chosen;

    uint32_t vk_swapchain_img_num;

    VkImage *vk_swapchain_imgs;
    size_t vk_image_array_layers;
    VkImageView *vk_image_view;

    VkRenderPass vk_render_pass;

    VkFramebuffer *vk_framebuffer;

    char *vertexShaderCode;
    char *fragmentShaderCode;

    VkShaderModule vertexShaderModule;
    VkShaderModule fragmentShaderModule;

    VkPipelineLayout vk_pipeline_layout;
    VkPipeline vk_pipeline;

    VkCommandPool vk_command_pool;

    VkCommandBuffer *vk_command_buf;

    size_t vk_max_frames;
    VkSemaphore *vk_wait_semaphore;
    VkSemaphore *vk_signal_semaphore;
    VkFence *vk_front_fences;
    VkFence *vk_back_fences;

    VkBuffer vertexBuffer;

    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;

    VkDeviceMemory indexBufferMemory;

    uint32_t vertex_ct;

    VkVertexInputBindingDescription *vbindingdesc;
    VkVertexInputAttributeDescription *vattribdesc;
} vulcano_struct;

typedef struct {
    vec2 pos;
    vec3 color;
} Vertex;

extern Vertex vertices[];

/* Function Definitions */
int vulkan_init(vulcano_struct *vulcano_state);
int vulkan_exit(vulcano_struct *vulcano_state);
void render(vulcano_struct *vulcano_state);
