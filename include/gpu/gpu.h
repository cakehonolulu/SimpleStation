#ifndef GPU_H
#define GPU_H

#include <simplestation.h>
#include <stdlib.h>
#include <stdint.h>

/* Function definitions */
uint8_t m_gpu_init(m_simplestation_state *m_simplestation);

uint32_t m_gpu_get_status(m_simplestation_state *m_simplestation);
uint32_t m_gpu_get_read(m_simplestation_state *m_simplestation);
uint32_t m_gpu_get_into_status(m_simplestation_state *m_simplestation);
uint8_t m_gpu_set_horizontal_res(uint8_t m_hoz_res1, uint8_t m_hoz_res2);

void m_gpu_gp0(uint32_t m_value, m_simplestation_state *m_simplestation);
void m_gpu_gp0_handler(m_simplestation_state *m_simplestation);

void m_gpu_gp1(uint32_t m_value, m_simplestation_state *m_simplestation);

void m_gpu_exit(m_simplestation_state *m_simplestation);

/* GPU Opcodes */

/* GP0 */

// 0x01
void m_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x02
void m_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x28
void m_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x2C
void m_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x30
void m_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x38
void m_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x68
void m_gpu_drawmonochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation);

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

/* GP1 */

// 0x00
void m_gpu_reset(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x01
void m_gpu_reset_command_buffer(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x02
void m_gpu_acknowledge_interrupt(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x03
void m_gpu_set_display_enabled(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x04
void m_gpu_set_dma_direction(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x05
void m_gpu_set_display_vram_start(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x06
void m_gpu_set_display_horizontal_range(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x07
void m_gpu_set_display_vertical_range(uint32_t m_value, m_simplestation_state *m_simplestation);

// 0x08
void m_gpu_set_display_mode(uint32_t m_value, m_simplestation_state *m_simplestation);

#endif /* GPU_H */