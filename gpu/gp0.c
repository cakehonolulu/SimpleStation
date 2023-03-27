#include <gpu/gp0.h>
#include <ui/termcolour.h>

extern renderstack_t renderstack;

void m_gpu_gp0_handler(m_simplestation_state *m_simplestation)
{
    switch (m_simplestation->m_gpu->m_gp0_cmd_ins)
    {
            case 0x00:
                break;
            
            case 0x01:
                renderstack.gpu_clear_cache(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x02:
                renderstack.gpu_fill_rect(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;
                        
            case 0x20:
                m_gpu_draw_opaque_three_point_monochrome_poly(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x28:
                renderstack.gpu_draw_monochrome_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;
            
            case 0x29:
                renderstack.gpu_draw_monochrome_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;
            
            case 0x2A:
                // Semitransparent
                renderstack.gpu_draw_monochrome_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x2B:
                // Semitransparent
                renderstack.gpu_draw_monochrome_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x2C:
                renderstack.gpu_draw_texture_blend_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x2D:
                renderstack.gpu_draw_texture_raw_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x2F:
                renderstack.gpu_draw_texture_raw_semitransparent_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x30:
                renderstack.gpu_draw_shaded_opaque_triangle(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x38:
                renderstack.gpu_draw_shaded_opaque_quad(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x3c:
                operationGp0TexturedShadedFourPointSemiTransparentTextureBlending(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x3e:
                operationGp0TexturedShadedFourPointSemiTransparentTextureBlending(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x40:
                renderstack.gpu_draw_monochrome_opaque_line(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;
            
            case 0x60:
                // FIND
                gpu_draw_monochrome_variable_size_rect(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x63:
                renderstack.gpu_draw_texture_blend_variable_size_rect(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x64:
                renderstack.gpu_draw_texture_blend_variable_size_rect(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x65:
                renderstack.gpu_draw_texture_raw_variable_size_rect(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x66:
                gpu_draw_texture_semi_transparent_opaque_texture_blend(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x68:
                renderstack.gpu_draw_monochrome_opaque_1x1(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x75:
                rect8by8(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0x7C:
                // TODO
                break;

            case 0x80:
                break;

            case 0xA0:
                renderstack.gpu_image_draw(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
                break;

            case 0xC0:
                renderstack.gpu_image_store(m_simplestation->m_gpu->m_gp0_instruction, m_simplestation);
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
        
            case 0x20:
                m_simplestation->m_gpu->m_gp0_words_remaining = 4;
                break;

            case 0x28:
                m_simplestation->m_gpu->m_gp0_words_remaining = 5;
                break;

            case 0x29:
                m_simplestation->m_gpu->m_gp0_words_remaining = 5;
                break;

            case 0x2A:
                m_simplestation->m_gpu->m_gp0_words_remaining = 5;
                break;

            case 0x2B:
                m_simplestation->m_gpu->m_gp0_words_remaining = 5;
                break;

            case 0x2C:
                m_simplestation->m_gpu->m_gp0_words_remaining = 9;
                break;

            case 0x2D:
                m_simplestation->m_gpu->m_gp0_words_remaining = 9;
                break;

            case 0x2F:
                m_simplestation->m_gpu->m_gp0_words_remaining = 9;
                break;

            case 0x30:
                m_simplestation->m_gpu->m_gp0_words_remaining = 6;
                break;

            case 0x38:
                m_simplestation->m_gpu->m_gp0_words_remaining = 8;
                break;

            case 0x3C:
                m_simplestation->m_gpu->m_gp0_words_remaining = 12;
                break;

            case 0x3E:
                m_simplestation->m_gpu->m_gp0_words_remaining = 12;
                break;
                
            case 0x40:
                m_simplestation->m_gpu->m_gp0_words_remaining = 3;
                break;

            case 0x60:
                m_simplestation->m_gpu->m_gp0_words_remaining = 3;
                break;

            case 0x63:
                m_simplestation->m_gpu->m_gp0_words_remaining = 4;
                break;

            case 0x64:
                m_simplestation->m_gpu->m_gp0_words_remaining = 4;
                break;

            case 0x65:
                m_simplestation->m_gpu->m_gp0_words_remaining = 4;
                break;

            case 0x66:
                m_simplestation->m_gpu->m_gp0_words_remaining = 4;
                break;

            case 0x68:
                m_simplestation->m_gpu->m_gp0_words_remaining = 2;
                break;

            case 0x75:
                m_simplestation->m_gpu->m_gp0_words_remaining = 3;
                break;

            case 0x7C:
                m_simplestation->m_gpu->m_gp0_words_remaining = 3;
                break;

            case 0x80:
                m_simplestation->m_gpu->m_gp0_words_remaining = 5;
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
                if (m_simplestation->renderer == OPENGL)
                {
                    glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, m_simplestation->m_gpu->write_buffer);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    m_sync_vram(m_simplestation);
                }

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
    m_simplestation->m_gpu->m_drawing_area_top = ((m_val >> 10) & 0x3FF);
    m_renderer_update_display_area(m_simplestation);
}

void m_gpu_set_draw_area_bottom_right(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    uint32_t m_val = m_simplestation->m_gpu_command_buffer->m_buffer[0];
    m_simplestation->m_gpu->m_drawing_area_right = m_val & 0x3FF;
    m_simplestation->m_gpu->m_drawing_area_bottom = ((m_val >> 10) & 0x3FF);
    m_renderer_update_display_area(m_simplestation);
}

extern GLint uniform_offset;

void m_gpu_set_draw_offset(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    if (m_simplestation->renderer == OPENGL) draw(m_simplestation, false, false, false);
    
    uint16_t m_x = ((uint16_t) (m_value & 0x7FF));
    uint16_t m_y = ((uint16_t) ((m_value >> 11) & 0x7FF));


    if (m_simplestation->renderer == OPENGL) glUniform2i(uniform_offset, (GLint) (((int16_t) (m_x << 5)) >> 5), (GLint) (((int16_t) (m_y << 5)) >> 5));
}

void m_gpu_set_mask_bit(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu->m_force_set_mask_bit = ((m_value & 1) != 0);
    m_simplestation->m_gpu->m_preserve_masked_pixels = ((m_value & 2) != 0);
}
