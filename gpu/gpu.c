#include <gpu/gpu.h>
#include <gpu/command_buffer.h>
#include <gpu/imagebuffer.h>
#include <gpu/renderer.h>
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
            m_renderer_init(m_simplestation);
            imageBuffer_Create(m_simplestation);
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

void m_gpu_gp0_handler(m_simplestation_state *m_simplestation)
{
    switch (m_simplestation->m_gpu->m_gp0_cmd_ins)
    {
            case 0x00:
                break;
            
            case 0x01:
                m_gpu_clear_cache(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x02:
                m_gpu_fill_rect(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;
            
            case 0x28:
                m_gpu_draw_monochrome_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;
            
            case 0x2C:
                m_gpu_draw_texture_blend_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x30:
                m_gpu_draw_shaded_opaque_triangle(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x38:
                m_gpu_draw_shaded_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x68:
                m_gpu_drawmonochrome_opaque_1x1(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xA0:
                m_gpu_image_draw(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xC0:
                m_gpu_image_store(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
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

extern GLuint m_psx_vram_texel;
uint32_t m_current_idx = 0;

void m_gpu_gp0(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_gp0_instruction = (m_value >> 24) & 0xFF;
    
    if (m_simplestation->m_gpu->m_gp0_words_remaining == 0)
    {
        m_gpu_command_buffer_clear(m_simplestation);
        
        m_simplestation->m_gpu->m_gp0_cmd_ins = m_simplestation->m_gpu->m_gp0_instruction;

        switch (m_simplestation->m_gpu->m_gp0_instruction)
        {
            case 0x00:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            case 0x01:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            case 0x02:
                m_simplestation->m_gpu->m_gp0_words_remaining = 3;
                break;

            case 0x28:
                m_simplestation->m_gpu->m_gp0_words_remaining = 5;
                break;

            case 0x2C:
                m_simplestation->m_gpu->m_gp0_words_remaining = 9;
                break;

            case 0x30:
                m_simplestation->m_gpu->m_gp0_words_remaining = 6;
                break;

            case 0x38:
                m_simplestation->m_gpu->m_gp0_words_remaining = 8;
                break;

            case 0x68:
                m_simplestation->m_gpu->m_gp0_words_remaining = 2;
                break;

            case 0xA0:
                m_simplestation->m_gpu->m_gp0_words_remaining = 3;
                break;

            case 0xC0:
                m_simplestation->m_gpu->m_gp0_words_remaining = 3;
                break;

            case 0xE1:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            case 0xE2:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            case 0xE3:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            case 0xE4:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            case 0xE5:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            case 0xE6:
                m_simplestation->m_gpu->m_gp0_words_remaining = 1;
                break;

            default:
                printf(RED "[GPU] gp0: Unhandled GP0 Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, m_simplestation->m_gpu->m_gp0_instruction, m_value);
                m_simplestation_exit(m_simplestation, 1);
                break;
        }
    }
    m_simplestation->m_gpu->m_gp0_words_remaining -= 1;

    switch (m_simplestation->m_gpu->m_gp0_mode)
    {
        case command:
            m_gpu_command_buffer_push_word(m_simplestation, m_value);

            if (m_simplestation->m_gpu->m_gp0_words_remaining == 0)
            {
                m_gpu_gp0_handler(m_simplestation);
            }
            break;

        case image_load:
            uint16_t width = m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xffff;
			uint16_t height = m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16;
			if (width == 0) width = 1024;
			if (height == 0) height = 512;
			width &= 0x3ff;
			height &= 0x1ff;
			int32_t x = m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xffff;
			int32_t y = m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16;

            m_simplestation->m_gpu_image_buffer->buffer[m_current_idx++] = m_value;
            
            if (m_simplestation->m_gpu->m_gp0_words_remaining == 0)
            {
                glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);
				glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, &m_simplestation->m_gpu_image_buffer->buffer[0]);
                glBindTexture(GL_TEXTURE_2D, 0);
                m_sync_vram(m_simplestation);
                for (int i = 0; i < (1024 * 512); i++) m_simplestation->m_gpu_image_buffer->buffer[i] = 0;
                m_current_idx = 0;
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

    m_sync_vram(m_simplestation);

    return;
}

extern int display_area_x, display_area_y, display_area_width, display_area_height;

void m_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation) {
    (void) m_value;

    uint32_t colour24 = m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xFFFFFF;
	uint16_t r = (colour24 & 0xFF) >> 3;
	uint16_t g = ((colour24 >> 8) & 0xFF) >> 3;
	uint16_t b = ((colour24 >> 16) & 0xFF) >> 3;

    glClearColor(r, g, b, 1.f);


    display_area_x = m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xffff;
    display_area_y = m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16;
    display_area_width = m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xffff;
    display_area_height = m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16;

    draw(m_simplestation, true);
}

void m_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    Colour col = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(Vertex));
    memset(&v2, 0, sizeof(Vertex));
    memset(&v3, 0, sizeof(Vertex));
    memset(&v4, 0, sizeof(Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col;
    v1.drawTexture = 0;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v2.colour = col;
    v2.drawTexture = 0;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v3.colour = col;
    v3.drawTexture = 0;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v4.colour = col;
    v4.drawTexture = 0;

    put_quad(v1, v2, v3, v4, m_simplestation);
}

void m_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    Colour col = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    ClutAttr clut = clutattr_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
	TexPage texPage = texpage_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	TextureColourDepth texDepth = tcd_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	GLubyte blend = (GLubyte) BlendTexture;

    Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(Vertex));
    memset(&v2, 0, sizeof(Vertex));
    memset(&v3, 0, sizeof(Vertex));
    memset(&v4, 0, sizeof(Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col;
    v1.texPage = texPage;
    v1.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v1.clut = clut;
    v1.texDepth = texDepth;
    v1.blendMode = blend;
    v1.drawTexture = 1;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col;
    v2.texPage = texPage;
    v2.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v2.clut = clut;
    v2.texDepth = texDepth;
    v2.blendMode = blend;
    v2.drawTexture = 1;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col;
    v3.texPage = texPage;
    v3.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[6]);
    v3.clut = clut;
    v3.texDepth = texDepth;
    v3.blendMode = blend;
    v3.drawTexture = 1;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[7]);
    v4.colour = col;
    v4.texPage = texPage;
    v4.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[8]);
    v4.clut = clut;
    v4.texDepth = texDepth;
    v4.blendMode = blend;
    v4.drawTexture = 1;

    put_quad(v1, v2, v3, v4, m_simplestation);
}

void m_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    Vertex v1, v2, v3;

    memset(&v1, 0, sizeof(Vertex));
    memset(&v2, 0, sizeof(Vertex));
    memset(&v3, 0, sizeof(Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);
    v1.drawTexture = 0;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v2.drawTexture = 0;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v3.drawTexture = 0;

    put_triangle(v1, v2, v3, m_simplestation);
    
    //printf(CYAN "[OPENGL] Draw Shaded Opaque Triangle\n" NORMAL);
}

void m_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(Vertex));
    memset(&v2, 0, sizeof(Vertex));
    memset(&v3, 0, sizeof(Vertex));
    memset(&v4, 0, sizeof(Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);
    v1.drawTexture = 0;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v2.drawTexture = 0;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v3.drawTexture = 0;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[7]);
    v4.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[6]);
    v4.drawTexture = 0;

    put_quad(v1, v2, v3, v4, m_simplestation);
    //printf(CYAN "[OPENGL] Draw Shaded Opaque Quadrilateral\n" NORMAL);
}

void m_gpu_drawmonochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    Rectangle r0;
    RectWidthHeight r0_wh;

    memset(&r0, 0, sizeof(Rectangle));

    r0_wh.width = 1;
    r0_wh.height = 1;

    r0.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
	r0.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);
	r0.widthHeight = r0_wh;

    put_rect(r0, m_simplestation);
}

void m_gpu_image_draw(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    uint32_t m_resolution = m_simplestation->m_gpu_command_buffer->m_buffer[2];

    uint16_t m_width = m_resolution & 0xFFFF;

    uint16_t m_height = m_resolution >> 16;

    uint32_t m_image_sz = ((m_height * m_width) + 1) & ~1;

    m_simplestation->m_gpu->m_gp0_words_remaining = m_image_sz / 2;

    m_simplestation->m_gpu->m_gp0_mode = image_load;

    if (m_simplestation->m_gpu->m_gp0_words_remaining > 0)
    {
        //imageBuffer_Reset(m_simplestation, m_x, m_y, m_width, m_height);
    }
}

void m_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation)
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
    uint32_t m_val = m_simplestation->m_gpu_command_buffer->m_buffer[0];
    m_simplestation->m_gpu->m_drawing_area_left = m_val & 0x3FF;
    m_simplestation->m_gpu->m_drawing_area_top = ((m_val >> 10) & 0x1FF);
    m_renderer_update_display_area(m_simplestation);
}

void m_gpu_set_draw_area_bottom_right(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    uint32_t m_val = m_simplestation->m_gpu_command_buffer->m_buffer[0];
    m_simplestation->m_gpu->m_drawing_area_right = m_val & 0x3FF;
    m_simplestation->m_gpu->m_drawing_area_bottom = ((m_val >> 10) & 0x1FF);
    m_renderer_update_display_area(m_simplestation);
}

extern GLint uniform_offset;

void m_gpu_set_draw_offset(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    draw(m_simplestation, false);
    
    uint16_t m_x = ((uint16_t) (m_value & 0x7FF));
    uint16_t m_y = ((uint16_t) ((m_value >> 11) & 0x7FF));


    glUniform2i(uniform_offset, (GLint) (((int16_t) (m_x << 5)) >> 5), (GLint) (((int16_t) (m_y << 5)) >> 5));
}

void m_gpu_set_mask_bit(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_force_set_mask_bit = ((m_value & 1) != 0);
    m_simplestation->m_gpu->m_preserve_masked_pixels = ((m_value & 2) != 0);
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
    m_simplestation->m_gpu->m_gp0_mode = command;
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
