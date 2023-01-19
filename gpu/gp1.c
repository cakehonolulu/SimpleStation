
#include <gpu/gp1.h>
#include <ui/termcolour.h>

void m_gpu_gp1(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    uint32_t m_opcode = (m_value >> 24) & 0xFF;

    switch (m_opcode)
    {
        case 0x00:
            m_gpu_reset(m_value, m_simplestation);
            break;

        case 0x01:
            m_gpu_reset_command_buffer(m_value, m_simplestation);
            break;

        case 0x02:
            m_gpu_acknowledge_interrupt(m_value, m_simplestation);
            break;

        case 0x03:
            m_gpu_set_display_enabled(m_value, m_simplestation);
            break;

        case 0x04:
            m_gpu_set_dma_direction(m_value, m_simplestation);
            break;

        case 0x05:
            m_gpu_set_display_vram_start(m_value, m_simplestation);
            break;

        case 0x06:
            m_gpu_set_display_horizontal_range(m_value, m_simplestation);
            break;

        case 0x07:
            m_gpu_set_display_vertical_range(m_value, m_simplestation);
            break;

        case 0x08:
            m_gpu_set_display_mode(m_value, m_simplestation);
            break;

        case 0x10:
            // TODO
            break;

        default:
            printf(RED "[GPU] gp1: Unhandled GP1 Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, m_opcode, m_value);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }
}

/* GP1 Commands */

void m_gpu_reset(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    m_simplestation->m_gpu->m_page_base_x = 0;
	m_simplestation->m_gpu->m_page_base_y = 0;
	m_simplestation->m_gpu->m_semitransparency = 0;
    m_simplestation->m_gpu->m_texture_depth = t4bit;
	m_simplestation->m_gpu->m_texture_window_x_mask = 0;
	m_simplestation->m_gpu->m_texture_window_y_mask = 0;
	m_simplestation->m_gpu->m_texture_window_x_offset = 0;
	m_simplestation->m_gpu->m_texture_window_y_offset = 0;
    m_simplestation->m_gpu->m_dithering = false;
    m_simplestation->m_gpu->m_draw_to_display = false;
    m_simplestation->m_gpu->m_texture_disable = false;
	m_simplestation->m_gpu->m_rectangle_texture_x_flip = false;
	m_simplestation->m_gpu->m_rectangle_texture_y_flip = false;
	m_simplestation->m_gpu->m_drawing_area_left = 0;
	m_simplestation->m_gpu->m_drawing_area_top = 0;
	m_simplestation->m_gpu->m_drawing_area_right = 0;
	m_simplestation->m_gpu->m_drawing_area_bottom = 0;
	m_simplestation->m_gpu->m_drawing_x_offset = 0;
	m_simplestation->m_gpu->m_drawing_y_offset = 0;
    m_simplestation->m_gpu->m_force_set_mask_bit = false;
    m_simplestation->m_gpu->m_preserve_masked_pixels = false;
    m_simplestation->m_gpu->m_dma_direction = off;
    m_simplestation->m_gpu->m_display_disabled = true;
	m_simplestation->m_gpu->m_display_vram_x_start = 0;
	m_simplestation->m_gpu->m_display_vram_y_start = 0;
    m_simplestation->m_gpu->m_horizontal_resolution = m_gpu_set_horizontal_res(0);
    m_simplestation->m_gpu->m_vertical_resolution = VRes240;
    m_simplestation->m_gpu->m_video_mode = ntsc;
    m_simplestation->m_gpu->m_interlaced = false;
	m_simplestation->m_gpu->m_display_horizontal_start = 0x200;
	m_simplestation->m_gpu->m_display_horizontal_end = 0xC00;
	m_simplestation->m_gpu->m_display_line_start = 0x10;
	m_simplestation->m_gpu->m_display_line_end = 0x100;
    m_simplestation->m_gpu->m_display_depth = d15bits;
}

void m_gpu_reset_command_buffer(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    m_gpu_command_buffer_clear(m_simplestation);
    m_simplestation->m_gpu->m_gp0_words_remaining = 0;
    m_simplestation->m_gpu->m_gp0_write_mode = command;
}

void m_gpu_acknowledge_interrupt(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    
    m_simplestation->m_gpu->m_interrupt = false;
}

void m_gpu_set_display_enabled(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_display_disabled = ((m_value & 1) != 0);
}

void m_gpu_set_dma_direction(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    switch (m_value & 3)
    {
        case 0:
            m_simplestation->m_gpu->m_dma_direction = off;
            break;

        case 1:
            m_simplestation->m_gpu->m_dma_direction = fifo;
            break;

        case 2:
            m_simplestation->m_gpu->m_dma_direction = cpu_to_gp0;
            break;

        case 3:
            m_simplestation->m_gpu->m_dma_direction = video_to_cpu;
            break;

        default:
            __builtin_unreachable();
            break;
    }
}

void m_gpu_set_display_vram_start(uint32_t m_value, m_simplestation_state *m_simplestation)
{
	m_simplestation->m_gpu->m_display_vram_x_start = ((uint16_t) (m_value & 0x3FE));
	m_simplestation->m_gpu->m_display_vram_y_start = ((uint16_t) ((m_value >> 10) & 0x1FF));
}

void m_gpu_set_display_horizontal_range(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_display_horizontal_start = ((uint16_t) (m_value & 0x3FF));
    m_simplestation->m_gpu->m_display_horizontal_end = ((uint16_t) ((m_value >> 10) & 0x3FF));
}

void m_gpu_set_display_vertical_range(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_display_line_start = ((uint16_t) (m_value & 0x3FF));
    m_simplestation->m_gpu->m_display_line_end = ((uint16_t) ((m_value >> 10) & 0x3FF));
}

void m_gpu_set_display_mode(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    //printf("Current hres: %d ; vres: %d\n", m_simplestation->m_gpu->m_horizontal_resolution, m_simplestation->m_gpu->m_vertical_resolution);
    
    m_simplestation->m_gpu->m_horizontal_resolution = m_gpu_set_horizontal_res(
           (((m_value & 3) << 1) | ((m_value >> 6) & 1)));
    
    m_simplestation->m_gpu->m_vertical_resolution = (verticalRes) (m_value & 0x4);;

    //printf("New hres: %d ; vres: %d\n", m_simplestation->m_gpu->m_horizontal_resolution, m_simplestation->m_gpu->m_vertical_resolution);

    if ((m_value & 0x8) != 0)
    {
        m_simplestation->m_gpu->m_video_mode = ntsc;
    }
    else
    {
        m_simplestation->m_gpu->m_video_mode = pal;
    }

    if ((m_value & 0x20) != 0)
    {
        m_simplestation->m_gpu->m_display_depth = d24bits;
    }
    else
    {
        m_simplestation->m_gpu->m_display_depth = d15bits;
    }

    m_simplestation->m_gpu->m_interlaced = ((m_value & 0x20) != 0);

    if ((m_value & 0x80) != 0)
    {
        printf(RED "[GP1] set_display_mode: Unsupported display mode (0x%08X)\n" NORMAL, m_value);
        m_simplestation_exit(m_simplestation, 1);
    }
}
