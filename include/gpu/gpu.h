#ifndef GPU_H
#define GPU_H

#include <simplestation.h>
#include <stdlib.h>
#include <stdint.h>

/* Function definitions */
uint8_t m_gpu_init(m_simplestation_state *m_simplestation);

uint32_t m_gpu_get_status(m_simplestation_state *m_simplestation);
uint32_t m_gpu_get_into_status(m_simplestation_state *m_simplestation);
uint8_t m_gpu_set_horizontal_res(uint8_t m_hoz_res1, uint8_t m_hoz_res2);
void m_gpu_gp0(uint32_t m_value, m_simplestation_state *m_simplestation);

void m_gpu_exit(m_simplestation_state *m_simplestation);

/* GPU Opcodes */

// 0xE1
void m_gpu_set_draw_mode(uint32_t m_value, m_simplestation_state *m_simplestation);

#endif /* GPU_H */