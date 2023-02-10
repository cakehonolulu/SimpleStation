#include <simplestation.h>
#include <gpu/gpu.h>
#include <gpu/command_buffer.h>
#include <stdio.h>
#include <stdint.h>

void m_gpu_gp1(uint32_t m_value, m_simplestation_state *m_simplestation);

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
