#include <renderer/vulkan.h>

#include <renderer/vulkan/vk_buffer.h>
#include <renderer/vulkan/vk_command.h>
#include <renderer/vulkan/vk_device.h>
#include <renderer/vulkan/vk_framebuffer.h>
#include <renderer/vulkan/vk_graphics_queue.h>
#include <renderer/vulkan/vk_image.h>
#include <renderer/vulkan/vk_instance.h>
#include <renderer/vulkan/vk_physical.h>
#include <renderer/vulkan/vk_pipeline.h>
#include <renderer/vulkan/vk_queue.h>
#include <renderer/vulkan/vk_shader.h>
#include <renderer/vulkan/vk_surface.h>
#include <renderer/vulkan/vk_swapchain.h>
#include <renderer/vulkan/vk_sync.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void init_vulkan_renderer(m_simplestation_state *m_simplestation)
{
    glfwInit();

    if (glfwVulkanSupported())
    {
        printf("Vulkan is supported, initializing\n");

        m_simplestation->vulcano_state = malloc(sizeof(vulcano_struct));

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            SDL_Log("Unable to initialize SDL2: %s", SDL_GetError());
            exit(1);
        }

        m_simplestation->vulcano_state->vulcano_window = SDL_CreateWindow(
                "Simplestation (SDL2) - Vulkan",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                640,
                480,
                SDL_WINDOW_VULKAN
                );
            
        bool vulkan_error = false;

        uint8_t retval = 1;

        m_simplestation->vulcano_state->instance = vk_create_instance(m_simplestation->vulcano_state, &vulkan_error);
        
        if (!vulkan_error)
        {
            int phys_dev_idx = vk_pick_physical_device(m_simplestation->vulcano_state, &vulkan_error);

            m_simplestation->vulcano_state->phys_dev = &m_simplestation->vulcano_state->physical_devices[phys_dev_idx];

            if (!vulkan_error && phys_dev_idx != -1)
            {
                uint32_t queue_family_number = vk_queue_get_prop_count_from_device(m_simplestation->vulcano_state->phys_dev);

                vk_queue_get_props_from_device(m_simplestation->vulcano_state, queue_family_number);

                vk_list_device_ext(m_simplestation->vulcano_state);

                m_simplestation->vulcano_state->device = vk_create_device(m_simplestation->vulcano_state, queue_family_number);

                m_simplestation->vulcano_state->vk_queue_family_idx = vk_graphics_queue_get_family_idx(m_simplestation->vulcano_state, queue_family_number);

                m_simplestation->vulcano_state->vk_queue_mode = vk_graphics_queue_get_mode(m_simplestation->vulcano_state, m_simplestation->vulcano_state->vk_queue_family_idx);

                m_simplestation->vulcano_state->vk_image_array_layers = 1;

                m_simplestation->vulcano_state->draw_queue = vk_graphics_queue_get_draw(m_simplestation->vulcano_state, m_simplestation->vulcano_state->vk_queue_family_idx);

                m_simplestation->vulcano_state->present_queue = vk_graphics_queue_get_presenting(m_simplestation->vulcano_state, m_simplestation->vulcano_state->vk_queue_family_idx, m_simplestation->vulcano_state->vk_queue_mode);

                if (SDL_Vulkan_CreateSurface(m_simplestation->vulcano_state->vulcano_window,
                    m_simplestation->vulcano_state->instance, &m_simplestation->vulcano_state->surface) != SDL_TRUE)
                {
                    printf(RED "[vulkan] init: Couldn't create Vulkan Surface!" NORMAL "\n");
                    printf(RED "         %s" NORMAL "\n", SDL_GetError());
                }
                else
                {
                    VkBool32 vk_surface_support = 0;
                
                    vkGetPhysicalDeviceSurfaceSupportKHR(*m_simplestation->vulcano_state->phys_dev, m_simplestation->vulcano_state->vk_queue_family_idx, m_simplestation->vulcano_state->surface, &vk_surface_support);

                    if (vk_surface_support)
                    {
                        vk_surface_prepare(m_simplestation->vulcano_state);

                        vk_swapchain_create(m_simplestation->vulcano_state);

                        vk_image_prepare(m_simplestation->vulcano_state);

                        vk_framebuffer_prepare(m_simplestation->vulcano_state);

                        vk_framebuffer_create(m_simplestation->vulcano_state);

                        uint32_t vertexShaderSize = 0;
                        char vertexShaderFileName[] = "vert_shader.spv";
                        m_simplestation->vulcano_state->vertexShaderCode = getShaderCode(vertexShaderFileName, &vertexShaderSize);

                        if (m_simplestation->vulcano_state->vertexShaderCode != NULL)
                        {
                            m_simplestation->vulcano_state->vertexShaderModule = createShaderModule(m_simplestation->vulcano_state, m_simplestation->vulcano_state->vertexShaderCode, vertexShaderSize);
                            
                            printf(MAGENTA BOLD "[vulkan] init: Vertex Shader Compiled Successfully!" NORMAL "\n");

                            uint32_t fragmentShaderSize = 0;
                            char fragmentShaderFileName[] = "frag_shader.spv";
                            m_simplestation->vulcano_state->fragmentShaderCode = getShaderCode(fragmentShaderFileName, &fragmentShaderSize);

                            if (m_simplestation->vulcano_state->fragmentShaderCode != NULL)
                            {
                                m_simplestation->vulcano_state->fragmentShaderModule = createShaderModule(m_simplestation->vulcano_state, m_simplestation->vulcano_state->fragmentShaderCode, fragmentShaderSize);
                                
                                printf(MAGENTA BOLD "[vulkan] init: Fragment Shader Compiled Successfully!" NORMAL "\n");

                                vk_create_pipeline(m_simplestation->vulcano_state);

                                vk_buffer_create(m_simplestation->vulcano_state);

                                vk_command_pool_init(m_simplestation->vulcano_state);

                                vk_sync_setup(m_simplestation->vulcano_state);

                                retval = 0;
                            }
                            else
                            {
                                printf(RED "[vulkan] init: Couldn't compile the fragment shaders!" NORMAL "\n");
                            }
                        }
                        else
                        {
                            printf(RED "[vulkan] init: Couldn't compile the vertex shaders!" NORMAL "\n");
                        }
                    }
                    else
                    {
                        printf(RED "[vulkan] init: Platform doesn't have VkSurfaceKHR support, exiting..." NORMAL "\n");
                    }
                }
            }
            else
            {
                printf(RED "[vulkan] init: Failed to pick a compute device, exiting..." NORMAL "\n");
            }
        }
        else
        {
            printf(RED "[vulkan] init: Failed to create Vulkan Instance, exiting..." NORMAL "\n");
        }
    }
    else
    {
        printf("Vulkan isn't supported, choose another renderer! Exiting...\n");
        exit(1);
    }
}
