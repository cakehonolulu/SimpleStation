#include <gpu/gpu.h>
#include <gpu/command_buffer.h>
#include <ui/termcolour.h>
#include <stdio.h>

uint8_t m_gpu_init(m_simplestation_state *m_simplestation)
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
            m_simplestation->m_gpu_command_buffer_state = ON;
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
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_preserve_masked_pixels) << 12;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_field) << 13;
    m_status |= ((uint32_t) m_simplestation->m_gpu->m_texture_disable) << 15;
    m_status |= m_gpu_get_into_status(m_simplestation);
    //m_status |= ((uint32_t) m_simplestation->m_gpu->m_vertical_resolution) << 19;
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

        case vram_to_cpu:
            m_status |= (((m_status >> 27) & 1) << 25);
            break;

        default:
            __builtin_unreachable();
            break;
    }

    return m_status;
}

uint32_t m_gpu_get_read(m_simplestation_state *m_simplestation)
{
    (void) m_simplestation;
    return 0;
}

uint32_t m_gpu_get_into_status(m_simplestation_state *m_simplestation)
{
    return (((uint32_t) m_simplestation->m_gpu->m_horizontal_resolution) << 16);
}

uint8_t m_gpu_set_horizontal_res(uint8_t m_hoz_res1, uint8_t m_hoz_res2)
{
    return ((m_hoz_res2 & 1) | ((m_hoz_res1 & 3) << 1));
}

void m_gpu_gp0_handler(m_simplestation_state *m_simplestation)
{
    switch (m_simplestation->m_gpu->m_gp0_cmd_ins)
    {
            case 0x00:
                break;
            
            case 0x01:
                m_gpu_clear_cache(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x28:
                m_gpu_draw_monochrome_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xE1:
                m_gpu_set_draw_mode(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xE2:
                m_gpu_set_texture_window(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xE3:
                m_gpu_set_draw_area_top_left(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xE4:
                m_gpu_set_draw_area_bottom_right(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xE5:
                m_gpu_set_draw_offset(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xE6:
                m_gpu_set_mask_bit(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            default:
                printf(RED "[GPU] gp0: Unhandled GP0 Opcode: 0x%02X\n" NORMAL, m_simplestation->m_gpu->m_gp0_cmd_ins);
                m_simplestation_exit(m_simplestation, 1);
                break;
    }
}

void m_gpu_gp0(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_gp0_instruction = (m_value >> 24) & 0xFF;
    
    if (m_simplestation->m_gpu->m_gp0_command_remaining == 0)
    {
        m_gpu_command_buffer_clear(m_simplestation);
        
        m_simplestation->m_gpu->m_gp0_cmd_ins = m_simplestation->m_gpu->m_gp0_instruction;

        switch (m_simplestation->m_gpu->m_gp0_instruction)
        {
            case 0x00:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            case 0x01:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            case 0x28:
                m_simplestation->m_gpu->m_gp0_command_remaining = 5;
                break;

            case 0xE1:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            case 0xE2:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            case 0xE3:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            case 0xE4:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            case 0xE5:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            case 0xE6:
                m_simplestation->m_gpu->m_gp0_command_remaining = 1;
                break;

            default:
                printf(RED "[GPU] gp0: Unhandled GP0 Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, m_simplestation->m_gpu->m_gp0_instruction, m_value);
                m_simplestation_exit(m_simplestation, 1);
                break;
        }
    }
    m_simplestation->m_gpu->m_gp0_command_remaining -= 1;

    switch (m_simplestation->m_gpu->m_gp0_mode)
    {
        case command:
            m_gpu_command_buffer_push_word(m_simplestation, m_value);

            if (m_simplestation->m_gpu->m_gp0_command_remaining == 0)
            {
                m_gpu_gp0_handler(m_simplestation);
            }
            break;

        case image_load:
            if (m_simplestation->m_gpu->m_gp0_command_remaining == 0)
            {
                m_simplestation->m_gpu->m_gp0_mode = command;
            }
            break;

        default:
            printf(YELLOW "[GP0] Unknown GP0 mode!\n" NORMAL);
            break;
    }
}

void m_gpu_gp1(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    uint32_t m_opcode = (m_value >> 24) & 0xFF;

    switch (m_opcode)
    {
        case 0x00:
            m_gpu_reset(m_value, m_simplestation);
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

        default:
            printf(RED "[GPU] gp1: Unhandled GP1 Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, m_opcode, m_value);
            m_simplestation_exit(m_simplestation, 1);
            break;
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

/* GP0 Commands */

void m_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    return;
}

void m_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    printf(CYAN "[OPENGL] Draw Monochrome Opaque Quadrilateral\n" NORMAL);
}

void m_gpu_set_draw_mode(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_page_base_x = ((uint8_t) (m_value & 0xF));
    m_simplestation->m_gpu->m_page_base_y = ((uint8_t) ((m_value >> 4) & 1));
    m_simplestation->m_gpu->m_semitransparency = ((uint8_t) ((m_value >> 5) & 3));

    switch ((m_value >> 7) & 3)
    {
        case 0:
            m_simplestation->m_gpu->m_texture_depth = t4bit;
            break;

        case 1:
            m_simplestation->m_gpu->m_texture_depth = t8bit;
            break;

        case 2:
            m_simplestation->m_gpu->m_texture_depth = t15bit;
            break;

        default:
            printf(RED "[GPU] set_draw_mode: Unknown texture depth value!\n" NORMAL);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

    m_simplestation->m_gpu->m_dithering = (((m_value >> 9) & 1) != 0);
    m_simplestation->m_gpu->m_draw_to_display = (((m_value >> 10) & 1) != 0);
    m_simplestation->m_gpu->m_texture_disable = (((m_value >> 11) & 1) != 0);
    m_simplestation->m_gpu->m_rectangle_texture_x_flip = (((m_value >> 12) & 1) != 0);
    m_simplestation->m_gpu->m_rectangle_texture_y_flip = (((m_value >> 13) & 1) != 0);
}

void m_gpu_set_texture_window(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_texture_window_x_mask = ((uint8_t) (m_value & 0x1F));
	m_simplestation->m_gpu->m_texture_window_y_mask = ((uint8_t) ((m_value >> 5) & 0x1F));
	m_simplestation->m_gpu->m_texture_window_x_offset = ((uint8_t) ((m_value >> 10) & 0x1F));
	m_simplestation->m_gpu->m_texture_window_y_offset = ((uint8_t) ((m_value >> 15) & 0x1F));
}

void m_gpu_set_draw_area_top_left(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_drawing_area_bottom = ((uint16_t) ((m_value >> 10) & 0x3FF));
    m_simplestation->m_gpu->m_drawing_area_right = ((uint16_t) (m_value & 0x3FF));
}

void m_gpu_set_draw_area_bottom_right(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_drawing_area_top = ((uint16_t) ((m_value >> 10) & 0x3FF));
    m_simplestation->m_gpu->m_drawing_area_left = ((uint16_t) (m_value & 0x3FF));
}

void m_gpu_set_draw_offset(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    uint16_t m_x = ((uint16_t) (m_value & 0x7FF));
    uint16_t m_y = ((uint16_t) ((m_value >> 11) & 0x7FF));

    m_simplestation->m_gpu->m_drawing_x_offset = ((int16_t) (m_x << 5)) >> 5;
    m_simplestation->m_gpu->m_drawing_y_offset = ((int16_t) (m_y << 5)) >> 5;
}

void m_gpu_set_mask_bit(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_force_set_mask_bit = ((m_value & 1) != 0);
    m_simplestation->m_gpu->m_preserve_masked_pixels = ((m_value & 2) != 0);
}

/* GP1 Commands */

void m_gpu_reset(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_value = (uint32_t) m_simplestation->m_gpu->m_field;

    memset(m_simplestation->m_gpu, 0, sizeof(m_psx_gpu_t));

    m_simplestation->m_gpu->m_field = m_value;

    m_simplestation->m_gpu->m_interlaced = true;

    m_simplestation->m_gpu->m_display_horizontal_start = 0x200;
	m_simplestation->m_gpu->m_display_horizontal_end = 0xC00;
	m_simplestation->m_gpu->m_display_line_start = 0x10;
	m_simplestation->m_gpu->m_display_line_end = 0x100;
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
            m_simplestation->m_gpu->m_dma_direction = vram_to_cpu;
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
    m_simplestation->m_gpu->m_display_horizontal_start = ((uint16_t) (m_value & 0xFFF));
    m_simplestation->m_gpu->m_display_horizontal_end = ((uint16_t) ((m_value >> 12) & 0xFFF));
}

void m_gpu_set_display_vertical_range(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_display_line_start = ((uint16_t) (m_value & 0x3FF));
    m_simplestation->m_gpu->m_display_line_end = ((uint16_t) ((m_value >> 10) & 0x3FF));
}

void m_gpu_set_display_mode(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_horizontal_resolution = m_gpu_set_horizontal_res(
            ((uint8_t) (m_value & 3)), ((uint8_t) ((m_value >> 6) & 1)));
    
    if ((m_value & 0x4) != 0)
    {
        m_simplestation->m_gpu->m_vertical_resolution = y240lines;
    }
    else
    {
        m_simplestation->m_gpu->m_vertical_resolution = y480lines;
    }

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
