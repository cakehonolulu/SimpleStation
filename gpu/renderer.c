#include <gpu/renderer.h>
#include <vk_instance.h>
#include <vk_image.h>
#include <vk_command.h>
#include <vk_framebuffer.h>
#include <vk_physical.h>
#include <vk_graphics_queue.h>
#include <vk_buffer.h>
#include <vk_queue.h>
#include <vk_surface.h>
#include <vk_swapchain.h>
#include <vk_shader.h>
#include <vk_pipeline.h>
#include <vk_sync.h>
#include <vk_device.h>

SDL_Window   *m_window;

uint8_t m_renderer_init(m_simplestation_state *m_simplestation)
{
	m_simplestation->vulcano_state = malloc(sizeof(vulcano_struct));

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL2: %s", SDL_GetError());
        return EXIT_FAILURE;
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

            m_simplestation->vulcano_state->device = vk_create_device(m_simplestation->vulcano_state, queue_family_number);

            vk_list_device_ext(m_simplestation->vulcano_state);

            m_simplestation->vulcano_state->vk_queue_family_idx = vk_graphics_queue_get_family_idx(m_simplestation->vulcano_state, queue_family_number);

            m_simplestation->vulcano_state->vk_queue_mode = vk_graphics_queue_get_mode(m_simplestation->vulcano_state, m_simplestation->vulcano_state->vk_queue_family_idx);

            m_simplestation->vulcano_state->vk_image_array_layers = 1;

            m_simplestation->vulcano_state->draw_queue = vk_graphics_queue_get_draw(m_simplestation->vulcano_state, m_simplestation->vulcano_state->vk_queue_family_idx);

	        m_simplestation->vulcano_state->present_queue = vk_graphics_queue_get_presenting(m_simplestation->vulcano_state, m_simplestation->vulcano_state->vk_queue_family_idx, m_simplestation->vulcano_state->vk_queue_mode);

            SDL_Vulkan_CreateSurface(m_simplestation->vulcano_state->vulcano_window,
                (SDL_vulkanInstance) m_simplestation->vulcano_state->instance, (SDL_vulkanSurface *) &m_simplestation->vulcano_state->surface);

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

                        //retval = 0;
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
        else
        {
            printf(RED "[vulkan] init: Failed to pick a compute device, exiting..." NORMAL "\n");
        }
    }
    else
    {
        printf(RED "[vulkan] init: Failed to create Vulkan Instance, exiting..." NORMAL "\n");
    }

	return 0;
}

uint32_t current_frame = 0;

unsigned int count_vertices = 0;

void draw(m_simplestation_state *m_simplestation, bool clear_colour) {
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

		count_vertices = 0;
	    vkDeviceWaitIdle(m_simplestation->vulcano_state->device);

	/* Off-screen Framebuffer */

	// Bind to FBO
	/*glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Use off-screen shaders
	glUseProgram(program);

	// Bind to off-screen VAO
	glBindVertexArray(m_vao);

	// Bind to off-screen VVBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VERTEX_BUFFER_LEN, m_vertex_buffer, GL_DYNAMIC_DRAW);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_gpu_vram, 0);

	if (clear_colour)
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	glViewport(0, 0, 1024, 512);
	
	// Off-screen shaders sample-off the off-screen VRAM Texture
	glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);

	// Draw the scene
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (count_vertices));

	glViewport(0, 0, 640, 480);
	
	// Copy the display area from the VRAM off to the on-screen texture
	glBindTexture(GL_TEXTURE_2D, m_window_texture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 640, 480, 0);

	// Clear the vertex count
	count_vertices = 0;

	// Bind GL_COLOR_ATTACHMENT0 w/the on-screen final texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_window_texture, 0);

	/* On-screen Framebuffer */

	// Bind to FBO
	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Use on-screen shaders
	glUseProgram(fb_program);

	// Bind to on-screen VAO
	glBindVertexArray(output_window_vao);

	// Bind to on-screen VBO
	glBindBuffer(GL_ARRAY_BUFFER, output_window_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(output_window_vertices), &output_window_vertices, GL_STATIC_DRAW);

	// Draw data-off the custom Framebuffer's Texture (GL_COLOR_ATTACHMENT0)
	glBindTexture(GL_TEXTURE_2D, m_window_texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);*/
}

void m_sync_vram(m_simplestation_state *m_simplestation)
{
	(void) m_simplestation;

    /*glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 640, 480);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_vram_texel, 0);
	glBindTexture(GL_TEXTURE_2D, m_psx_gpu_vram);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 1024, 512, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, 1024, 512);*/
}


void display(m_simplestation_state *m_simplestation) {
  draw(m_simplestation, false);
  //SDL_GL_SwapWindow(m_window);
}

void m_window_changetitle(char *buffer)
{
	SDL_SetWindowTitle(m_window, buffer);
}

void pos_from_gp0(uint32_t val, vec2 *vec)
{
	vec = (vec2) {(int16_t) (val & 0xFFFF), (int16_t) (val >> 16)};
}

void col_from_gp0(uint32_t value, vec3 *vec)
{
	vec = (vec3) {(uint8_t) (value & 0xFF), (uint8_t) ((value >> 8) & 0xFF), (uint8_t) ((value >> 16) & 0xFF)};
}

TexPage texpage_from_gp0(uint32_t value)
{
	TexPage texp;
	texp.xBase = ((value >> 16) & 0xF) * 64;
	texp.yBase = (((value >> 16) >> 4) & 1) * 256;
	return texp;
}

TexCoord texcoord_from_gp0(uint32_t value)
{
	TexCoord texc;
	texc.x = value & 0xFF;
	texc.y = (value >> 8) & 0xFF;
	return texc;
}

ClutAttr clutattr_from_gp0(uint32_t value)
{
	ClutAttr clut;
	clut.x = ((value >> 16) & 0x3F) * 16;
	clut.y = ((value >> 16) >> 6) & 0x1FF;
	return clut;
}

TextureColourDepth tcd_from_gp0(uint32_t value)
{
	TextureColourDepth tcd;
	tcd.depth = ((value >> 16) >> 7) & 0x3;
	return tcd;
}

TextureColourDepth tcd_from_val(textureColourDepthValue value)
{
	TextureColourDepth tcd;
	tcd.depth = (uint8_t) value;
	return tcd;
}

Colour color(uint8_t r, uint8_t g, uint8_t b) {
	Colour colorr;

	colorr.r = r;
	colorr.g = g;
	colorr.b = b;
	return colorr;
}

int put_triangle(Vertex v1, Vertex v2, Vertex v3, m_simplestation_state *m_simplestation) {
	Vertex vertices[] = {
		v1,
        v2,
        v3
	};

	void* data;

	//printf("Data: 0x%X, Data + = 0x%X\n", data, data + (count_vertices * sizeof(vertices)));
    vkMapMemory(m_simplestation->vulcano_state->device, m_simplestation->vulcano_state->vertexBufferMemory, 0, sizeof(Vertex) * VERTEX_BUFFER_LEN, 0, &data);
		memcpy(data + (count_vertices * sizeof(vertices)), vertices, sizeof(vertices));
        //memcpy(data, vertices, (size_t) sizeof(vertices));
    vkUnmapMemory(m_simplestation->vulcano_state->device, m_simplestation->vulcano_state->vertexBufferMemory);

	count_vertices += sizeof(vertices);

	/*if (count_vertices + 3 > VERTEX_BUFFER_LEN)
	{
		printf("Vertex attribute buffers full, forcing_draw\n");
		//draw();
	}

	m_vertex_buffer[count_vertices] = v1;
	count_vertices++;

	m_vertex_buffer[count_vertices] = v2;
	count_vertices++;
	
	m_vertex_buffer[count_vertices] = v3;
	count_vertices++;*/

	return 0;	
}

int put_quad(Vertex v1, Vertex v2, Vertex v3, Vertex v4, m_simplestation_state *m_simplestation) {
	put_triangle(v1, v2, v3, m_simplestation);
	put_triangle(v2, v3, v4, m_simplestation);
	return 0;
}
