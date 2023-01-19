#include <gpu/gp0.h>
#include <ui/termcolour.h>

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

            case 0x2D:
                m_gpu_draw_texture_raw_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x30:
                m_gpu_draw_shaded_opaque_triangle(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x38:
                m_gpu_draw_shaded_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x65:
                m_gpu_draw_texture_raw_variable_size_rect(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x68:
                m_gpu_draw_monochrome_opaque_1x1(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
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

            case 0x2D:
                m_simplestation->m_gpu->m_gp0_words_remaining = 9;
                break;

            case 0x30:
                m_simplestation->m_gpu->m_gp0_words_remaining = 6;
                break;

            case 0x38:
                m_simplestation->m_gpu->m_gp0_words_remaining = 8;
                break;

            case 0x65:
                m_simplestation->m_gpu->m_gp0_words_remaining = 4;
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

    switch (m_simplestation->m_gpu->m_gp0_write_mode)
    {
        case command:
            m_gpu_command_buffer_push_word(m_simplestation, m_value);

            if (m_simplestation->m_gpu->m_gp0_words_remaining == 0)
            {
                m_gpu_gp0_handler(m_simplestation);
            }
            break;

        case cpu_to_vram:
	{
            uint16_t width = m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xffff;
			uint16_t height = m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16;
            
            width = ((width - 1) & 0x3ff) + 1;
            height = ((height - 1) & 0x1ff) + 1;

			int32_t x = m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0x3ff;
			int32_t y = (m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16) & 0x1ff;;

            //if (m_value = 0x00) m_value = 0x0F;
            
            m_simplestation->m_gpu->write_buffer[m_current_idx++] = m_value;

            if (m_simplestation->m_gpu->m_gp0_words_remaining == 0)
            {
                glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);
				glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, m_simplestation->m_gpu->write_buffer);
                glBindTexture(GL_TEXTURE_2D, 0);
                m_sync_vram(m_simplestation);
                for (int i = 0; i < (1024 * 512); i++) m_simplestation->m_gpu->write_buffer[i] = 0;
                m_current_idx = 0;
                m_simplestation->m_gpu->m_gp0_write_mode = command;
            }

            break;
	}

        default:
            printf(YELLOW "[GP0] Unknown GP0 mode!\n" NORMAL);
            break;
    }
}


/* GP0 Commands */

void m_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    for (int i = 0; i < (1024 * 512); i++) m_simplestation->m_gpu->write_buffer[i] = 0;

    return;
}

extern int display_area_x, display_area_y, display_area_width, display_area_height;

void m_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation) {
    (void) m_value;

    uint32_t colour24 = m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xFFFFFF;
	float r = (colour24 & 0xFF) / 255.0f;
	float g = ((colour24 >> 8) & 0xFF) / 255.0f;
	float b = ((colour24 >> 16) & 0xFF) / 255.0f;

    glClearColor(r, g, b, 0.f);

    glEnable(GL_SCISSOR_TEST);

    display_area_x = m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xffff;
    display_area_y = m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16;
    display_area_width = m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xffff;
    display_area_height = m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16;

    glScissor(display_area_x, display_area_y, display_area_width, display_area_height);
    draw(m_simplestation, true, false);

    glDisable(GL_SCISSOR_TEST);
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

void m_gpu_draw_texture_raw_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
        (void) m_value;

    Colour col = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    ClutAttr clut = clutattr_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
	TexPage texPage = texpage_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	TextureColourDepth texDepth = tcd_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	GLubyte blend = (GLubyte) RawTexture;

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

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

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

void m_gpu_draw_texture_raw_variable_size_rect(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    Rectangle r0;

    memset(&r0, 0, sizeof(Rectangle));

    r0.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    r0.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);
    r0.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    r0.clut = clutattr_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    r0.blendMode = (GLubyte) RawTexture;
    r0.drawTexture = 1;
    r0.widthHeight = rwh_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);

	put_rect(r0, m_simplestation);
}

void m_gpu_draw_monochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation)
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

    m_width = ((m_width - 1) & 0x3ff) + 1;
    m_height = ((m_height - 1) & 0x1ff) + 1;

    uint32_t m_image_sz = ((m_height * m_width) + 1) & ~1;

    m_simplestation->m_gpu->m_gp0_words_remaining = m_image_sz / 2;

    m_simplestation->m_gpu->m_gp0_write_mode = cpu_to_vram;
}

void m_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    draw(m_simplestation, false, true);
    

    const uint32_t coords = m_simplestation->m_gpu_command_buffer->m_buffer[1];
    const uint32_t res = m_simplestation->m_gpu_command_buffer->m_buffer[2];
    // TODO: Sanitize this
    const auto x = coords & 0x3ff;
    const auto y = (coords >> 16) & 0x1ff;

    uint32_t width = res & 0xffff;
    uint32_t height = res >> 16;

    width = ((width - 1) & 0x3ff) + 1;
    height = ((height - 1) & 0x1ff) + 1;

    // The size of the texture in 16-bit pixels. If the number is odd, force align it up
    const uint32_t size = ((width * height) + 1) & ~1;

    m_simplestation->m_gpu->m_gp0_read_mode = vram_to_cpu;
    m_simplestation->m_gpu->m_vram_image_size = size / 2;

    m_simplestation->m_gpu->m_vram_image_index = 0;


    extern GLuint m_fbo, m_psx_gpu_vram, m_psx_vram_texel;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_gpu_vram, 0);

    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, m_simplestation->m_gpu->read_buffer);


    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_vram_texel, 0);

    glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, m_simplestation->m_gpu->read_buffer);
}

void m_gpu_set_draw_mode(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_page_base_x = ((uint8_t) (m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xF));
    m_simplestation->m_gpu->m_page_base_y = ((uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 4) & 1));
    m_simplestation->m_gpu->m_semitransparency = ((uint8_t) ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 5) & 3));

    switch ((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 7) & 3)
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

    m_simplestation->m_gpu->m_dithering = (((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 9) & 1) != 0);
    m_simplestation->m_gpu->m_draw_to_display = (((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 10) & 1) != 0);
    m_simplestation->m_gpu->m_texture_disable = (((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 11) & 1) != 0);
    m_simplestation->m_gpu->m_rectangle_texture_x_flip = (((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 12) & 1) != 0);
    m_simplestation->m_gpu->m_rectangle_texture_y_flip = (((m_simplestation->m_gpu_command_buffer->m_buffer[0] >> 13) & 1) != 0);
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
    draw(m_simplestation, false, false);
    
    uint16_t m_x = ((uint16_t) (m_value & 0x7FF));
    uint16_t m_y = ((uint16_t) ((m_value >> 11) & 0x7FF));


    glUniform2i(uniform_offset, (GLint) (((int16_t) (m_x << 5)) >> 5), (GLint) (((int16_t) (m_y << 5)) >> 5));
}

void m_gpu_set_mask_bit(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_force_set_mask_bit = ((m_value & 1) != 0);
    m_simplestation->m_gpu->m_preserve_masked_pixels = ((m_value & 2) != 0);
}
