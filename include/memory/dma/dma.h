#ifndef DMA_H
#define DMA_H

#include <simplestation.h>
#include <memory/dma/channel.h>
#include <ui/termcolour.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* Function Definitions */
void m_dma_init(m_simplestation_state *m_simplestation);

void m_dma_write(uint32_t m_addr, uint32_t m_value, m_simplestation_state *m_simplestation);
uint32_t m_dma_read(uint32_t m_addr, m_simplestation_state *m_simplestation);

void m_set_interrupt(uint32_t m_val, m_simplestation_state *m_simplestation);
bool m_irq(m_simplestation_state *m_simplestation);
uint32_t m_get_interrupt(m_simplestation_state *m_simplestation);

void m_dma_exit(m_simplestation_state *m_simplestation);

#endif /* DMA_H */