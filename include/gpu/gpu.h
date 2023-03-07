#ifndef GPU_H
#define GPU_H

#include <simplestation.h>
#include <renderer/renderer.h>
#include <stdlib.h>
#include <stdint.h>

/* Function definitions */
uint8_t m_gpu_init(m_simplestation_state *m_simplestation, renderstack_t *renderstack);

uint32_t m_gpu_get_status(m_simplestation_state *m_simplestation);
uint32_t m_gpu_get_read(m_simplestation_state *m_simplestation);
uint32_t m_gpu_get_into_status(m_simplestation_state *m_simplestation);
horizontalRes m_gpu_set_horizontal_res(uint8_t fields);
void m_gpu_exit(m_simplestation_state *m_simplestation);

#endif /* GPU_H */