#include <gpu/gpu.h>
#include <gpu/command_buffer.h>
#include <renderer/renderer.h>
#include <renderer/vulkan.h>
#include <ui/termcolour.h>
#include <stdio.h>

uint8_t m_gpu_init(m_simplestation_state *m_simplestation, renderstack_t *renderstack)
{
    uint8_t m_result = 0;

    m_simplestation->m_gpu = (m_psx_gpu_t *) malloc(sizeof(m_psx_gpu_t));

    if (m_simplestation->m_gpu)
    {
        memset(m_simplestation->m_gpu, 0, sizeof(m_psx_gpu_t));

        m_simplestation->m_gpu->m_field = top;

        if (m_gpu_command_buffer_init(m_simplestation))
        {
            printf(RED "[GPU] command_buffer_init: Failed to allocate PSX's GPU Command Buffer!\n" NORMAL);
            m_result = 1;
        }
        else
        {
            m_result = setup_renderer(m_simplestation, renderstack);
            
            if (m_result)
            {
                printf(RED "[GPU] m_gpu_init: Failed to initialize a renderer!\n" NORMAL);
            }
        }
    }
    else
    {
        printf("[GPU] init: Couldn't initialize PSX's GPU, exiting...");
        m_result = 1;
    }

    return m_result;
}

uint32_t m_gpu_get_status(m_simplestation_state *m_simplestation)
{
    uint32_t m_status = 0;
    m_status |= m_simplestation->m_gpu->m_page_base_x << 0;
    m_status |= m_simplestation->m_gpu->m_page_base_y << 4;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_semitransparency) << 5;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_texture_depth) << 7;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_dithering) << 9;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_draw_to_display) << 10;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_force_set_mask_bit) << 11;
    // HACK
    // PSX Shell doesn't crash anymore
    //m_status |= ((uint32_t) m_simplestation->m_gpu->m_preserve_masked_pixels) << 12;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_field) << 13;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_texture_disable) << 15;
    m_status |= m_gpu_get_into_status(m_simplestation);
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_vertical_resolution) << 19;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_video_mode) << 20;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_display_depth) << 21;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_interlaced) << 22;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_display_disabled) << 23;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_interrupt) << 24;

    // HACK
    // GPU Always Ready to Recieve Commands
    m_status |= 1 << 26;

    // Ready to send VRAM to CPU
    m_status |= 1 << 27;

    // Ready to recieve DMA block
    m_status |= 1 << 28;

    m_status |= ((uint32_t) m_simplestation->m_gpu->m_dma_direction) << 29;

    m_status |= 0 << 31;


    switch (m_simplestation->m_gpu->m_dma_direction)
    {
        case off:
            m_status |= ((0) << 25);
            break;

        case fifo:
            m_status |= ((1)  << 25);
            break;

        case cpu_to_gp0:
            m_status |= (((m_status >> 28) & 1) << 25);
            break;

        case video_to_cpu:
            m_status |= (((m_status >> 27) & 1) << 25);
            break;

        default:
            __builtin_unreachable();
            break;
    }

    return m_status;
}

void m_gpu_read_vram_data(uint32_t* destination, int size, m_simplestation_state *m_simplestation) {
    for (int i = 0; i < size; i++) {
        if (m_simplestation->m_gpu->m_gp0_read_mode == vram_to_cpu) {
            if (m_simplestation->m_gpu->m_vram_image_size == 1) m_simplestation->m_gpu->m_gp0_read_mode = command;
            *destination++ = m_simplestation->m_gpu->read_buffer[m_simplestation->m_gpu->m_vram_image_index++];
            m_simplestation->m_gpu->m_vram_image_size--;
        } else {
            return;
        }
    }
}

uint32_t m_gpu_get_read(m_simplestation_state *m_simplestation)
{
    (void) m_simplestation;

    uint32_t m_read = 0;

    if (m_simplestation->m_gpu->m_gp0_read_mode == vram_to_cpu)
    {
        m_gpu_read_vram_data(&m_read, 1, m_simplestation);
    }

    return m_read;
}

uint32_t m_gpu_get_into_status(m_simplestation_state *m_simplestation)
{
    return (((uint32_t) m_simplestation->m_gpu->m_horizontal_resolution) << 16);
}

horizontalRes m_gpu_set_horizontal_res(uint8_t fields)
{
	if (fields & 1)
	{
		return XRes368;
	}
	else
	{
		switch ((fields >> 1) & 0x3)
		{
			case 0: return XRes256;
			case 1: return XRes320;
			case 2: return XRes512;
			case 3: return XRes640;
		}
	}
}

void m_gpu_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_gpu_state)
    {
        free(m_simplestation->m_gpu);
    }

    m_simplestation->m_gpu_state = OFF;
}
