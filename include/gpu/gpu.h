#ifndef GPU_H
#define GPU_H

#include <simplestation.h>
#include <stdlib.h>
#include <stdint.h>

/* Function definitions */
uint8_t m_gpu_init(m_simplestation_state *m_simplestation);
void m_gpu_exit(m_simplestation_state *m_simplestation);

#endif /* GPU_H */