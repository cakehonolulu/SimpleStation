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

uint8_t init_vulkan_renderer(m_simplestation_state *m_simplestation)
{
    uint8_t retval = 1;

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

    return retval;
}
unsigned int vulkan_count_vertices = 0, current_frame = 0;

void vulkan_draw(m_simplestation_state *m_simplestation, bool clear_colour)
{
	(void) clear_colour;

	    vkDeviceWaitIdle(m_simplestation->vulcano_state->device);

	vkWaitForFences(m_simplestation->vulcano_state->device, 1, &m_simplestation->vulcano_state->vk_front_fences[current_frame], VK_TRUE, UINT64_MAX);
		uint32_t image_index = 0;

		vkAcquireNextImageKHR(m_simplestation->vulcano_state->device, m_simplestation->vulcano_state->vk_swapchain, UINT64_MAX, m_simplestation->vulcano_state->vk_wait_semaphore[current_frame], NULL, &image_index);

		//if(vulcano_state->vk_back_fences[image_index] != NULL)
        //{
            // FIXME
			//vkWaitForFences(vulcano_state->device, 1, &vulcano_state->vk_back_fences[image_index], VK_TRUE, UINT64_MAX);
		//}
		//vulcano_state->vk_back_fences[image_index] = vulcano_state->vk_front_fences[current_frame];

		VkPipelineStageFlags pipeline_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			NULL,
			1,
			&m_simplestation->vulcano_state->vk_wait_semaphore[current_frame],
			&pipeline_flags,
			1,
			&m_simplestation->vulcano_state->vk_command_buf[image_index],
			1,
			&m_simplestation->vulcano_state->vk_signal_semaphore[current_frame]
		};

		vkResetFences(m_simplestation->vulcano_state->device, 1, &m_simplestation->vulcano_state->vk_front_fences[current_frame]);
		vkQueueSubmit(m_simplestation->vulcano_state->draw_queue, 1, &submitInfo, m_simplestation->vulcano_state->vk_front_fences[current_frame]);

		VkPresentInfoKHR present_info = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			NULL,
			1,
			&m_simplestation->vulcano_state->vk_signal_semaphore[current_frame],
			1,
			&m_simplestation->vulcano_state->vk_swapchain,
			&image_index,
			NULL
		};
		vkQueuePresentKHR(m_simplestation->vulcano_state->present_queue, &present_info);

		current_frame = (current_frame + 1) % m_simplestation->vulcano_state->vk_max_frames;

		vulkan_count_vertices = 0;
	    vkDeviceWaitIdle(m_simplestation->vulcano_state->device);
}

int vulkan_put_triangle(Vulkan_Vertex v1, Vulkan_Vertex v2, Vulkan_Vertex v3, m_simplestation_state *m_simplestation)
{
	Vulkan_Vertex vertices[] = {
		v1,
        v2,
        v3
	};

	void* data;

    vkMapMemory(m_simplestation->vulcano_state->device, m_simplestation->vulcano_state->vertexBufferMemory, 0, sizeof(Vulkan_Vertex) * VERTEX_BUFFER_LEN, 0, &data);
		memcpy(data + (vulkan_count_vertices * sizeof(vertices)), vertices, sizeof(vertices));
    vkUnmapMemory(m_simplestation->vulcano_state->device, m_simplestation->vulcano_state->vertexBufferMemory);

	vulkan_count_vertices += sizeof(vertices);

	return 0;	
}

int vulkan_put_quad(Vulkan_Vertex v1, Vulkan_Vertex v2, Vulkan_Vertex v3, Vulkan_Vertex v4, m_simplestation_state *m_simplestation)
{
	vulkan_put_triangle(v1, v2, v3, m_simplestation);
	vulkan_put_triangle(v2, v3, v4, m_simplestation);
	return 0;
}

void vulkan_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    // TODO
}

void vulkan_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    // TODO
}

void vulkan_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    vec3 col = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 16) & 0xFF)
    };

    Vulkan_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(Vulkan_Vertex));
    memset(&v2, 0, sizeof(Vulkan_Vertex));
    memset(&v3, 0, sizeof(Vulkan_Vertex));
    memset(&v4, 0, sizeof(Vulkan_Vertex));
    
    vec2 pos1 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16)
    };


    memcpy(v1.pos, pos1, sizeof(vec2));
    memcpy(v1.color, col, sizeof(vec3));

    vec2 pos2 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16)
    };


    memcpy(v2.pos, pos2, sizeof(vec2));
    memcpy(v2.color, col, sizeof(vec3));

    vec2 pos3 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] >> 16)
    };

    memcpy(v3.pos, pos3, sizeof(vec2));
    memcpy(v3.color, col, sizeof(vec3));

    vec2 pos4 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[4] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[4] >> 16)
    };


    memcpy(v4.pos, pos4, sizeof(vec2));
    memcpy(v4.color, col, sizeof(vec3));

    vulkan_put_quad(v1, v2, v3, v4, m_simplestation);
}

void vulkan_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    vec3 col = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 16) & 0xFF)
    };

    Vulkan_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(Vulkan_Vertex));
    memset(&v2, 0, sizeof(Vulkan_Vertex));
    memset(&v3, 0, sizeof(Vulkan_Vertex));
    memset(&v4, 0, sizeof(Vulkan_Vertex));
    
    vec2 pos1 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16)
    };


    memcpy(v1.pos, pos1, sizeof(vec2));
    memcpy(v1.color, col, sizeof(vec3));

    vec2 pos2 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] >> 16)
    };


    memcpy(v2.pos, pos2, sizeof(vec2));
    memcpy(v2.color, col, sizeof(vec3));

    vec2 pos3 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[5] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[5] >> 16)
    };

    memcpy(v3.pos, pos3, sizeof(vec2));
    memcpy(v3.color, col, sizeof(vec3));

    vec2 pos4 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[7] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[7] >> 16)
    };


    memcpy(v4.pos, pos4, sizeof(vec2));
    memcpy(v4.color, col, sizeof(vec3));

    vulkan_put_quad(v1, v2, v3, v4, m_simplestation);
}

void vulkan_gpu_draw_texture_raw_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;
}

void vulkan_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    Vulkan_Vertex v1, v2, v3;

    memset(&v1, 0, sizeof(Vulkan_Vertex));
    memset(&v2, 0, sizeof(Vulkan_Vertex));
    memset(&v3, 0, sizeof(Vulkan_Vertex));
    
    vec2 pos1 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16)
    };

    vec3 col1 = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 16) & 0xFF)
    };

    memcpy(v1.pos, pos1, sizeof(vec2));
    memcpy(v1.color, col1, sizeof(vec3));

    vec2 pos2 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] >> 16)
    };

    vec3 col2 = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16) & 0xFF)
    };

    memcpy(v2.pos, pos2, sizeof(vec2));
    memcpy(v2.color, col2, sizeof(vec3));

    vec2 pos3 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[5] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[5] >> 16)
    };

    vec3 col3 = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[4] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[4] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[4] >> 16) & 0xFF)
    };

    memcpy(v3.pos, pos3, sizeof(vec2));
    memcpy(v3.color, col3, sizeof(vec3));

    vulkan_put_triangle(v1, v2, v3, m_simplestation);
}

void vulkan_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    Vulkan_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(Vulkan_Vertex));
    memset(&v2, 0, sizeof(Vulkan_Vertex));
    memset(&v3, 0, sizeof(Vulkan_Vertex));
    memset(&v4, 0, sizeof(Vulkan_Vertex));
    
    vec2 pos1 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16)
    };


    vec3 col1 = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 16) & 0xFF)
    };

    memcpy(v1.pos, pos1, sizeof(vec2));
    memcpy(v1.color, col1, sizeof(vec3));

    vec2 pos2 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[3] >> 16)
    };

    vec3 col2 = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16) & 0xFF)
    };

    memcpy(v2.pos, pos2, sizeof(vec2));
    memcpy(v2.color, col2, sizeof(vec3));

    vec2 pos3 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[5] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[5] >> 16)
    };

    vec3 col3 = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[4] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[4] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[4] >> 16) & 0xFF)
    };

    memcpy(v3.pos, pos3, sizeof(vec2));
    memcpy(v3.color, col3, sizeof(vec3));

    vec2 pos4 = {
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[7] & 0xFFFF),
        (int16_t) (m_simplestation->m_gpu_command_buffer->m_buffer[7] >> 16)
    };

    vec3 col4 = {
        (uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[6] & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[6] >> 8) & 0xFF),
        (uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[6] >> 16) & 0xFF)
    };

    memcpy(v4.pos, pos4, sizeof(vec2));
    memcpy(v4.color, col4, sizeof(vec3));

    vulkan_put_quad(v1, v2, v3, v4, m_simplestation);
}

void vulkan_gpu_draw_texture_raw_variable_size_rect(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;
}

void vulkan_gpu_draw_monochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;
}

void vulkan_gpu_image_draw(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    uint32_t m_resolution = m_simplestation->m_gpu_command_buffer->m_buffer[2];

    uint16_t m_width = m_resolution & 0xFFFF;

    uint16_t m_height = m_resolution >> 16;

    uint32_t m_image_sz = ((m_height * m_width) + 1) & ~1;

    m_simplestation->m_gpu->m_gp0_words_remaining = m_image_sz / 2;

    m_simplestation->m_gpu->m_gp0_write_mode = cpu_to_vram;

    // TODO
}

void vulkan_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

#ifndef DEBUG_GP0
    (void) m_simplestation;
#else
    uint32_t m_resolution = m_simplestation->m_gpu_command_buffer->m_buffer[2];

    uint16_t m_width = m_resolution & 0xFFFF;

    uint16_t m_height = m_resolution >> 16;

    printf(MAGENTA "[GP0] image_store: Unhandled image store size: %d, %d\n" NORMAL, m_height, m_width);
#endif
}


