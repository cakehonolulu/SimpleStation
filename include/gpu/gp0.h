#include <simplestation.h>
#include <renderer/renderer.h>
#include <stdint.h>

void m_gpu_gp0_handler(m_simplestation_state *m_simplestation);
void m_gpu_gp0(uint32_t m_value, m_simplestation_state *m_simplestation);

/* GP0 */

// 0x01
void m_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x02
void m_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x28
void m_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x2C
void m_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x2D
void m_gpu_draw_texture_raw_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x30
void m_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x38
void m_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x65
void m_gpu_draw_texture_raw_variable_size_rect(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x68
void m_gpu_draw_monochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xA0
void m_gpu_image_draw(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xC0
void m_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xE1
void m_gpu_set_draw_mode(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xE2
void m_gpu_set_texture_window(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xE3
void m_gpu_set_draw_area_top_left(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xE4
void m_gpu_set_draw_area_bottom_right(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xE5
void m_gpu_set_draw_offset(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0xE6
void m_gpu_set_mask_bit(uint32_t m_value, m_simplestation_state *m_simplestation);
