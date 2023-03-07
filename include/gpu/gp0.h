#include <simplestation.h>
#include <renderer/renderer.h>
#include <gpu/command_buffer.h>
#include <stdint.h>

void m_gpu_gp0_handler(m_simplestation_state *m_simplestation);
void m_gpu_gp0(uint32_t m_value, m_simplestation_state *m_simplestation);

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
