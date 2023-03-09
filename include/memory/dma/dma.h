#ifndef DMA_H
#define DMA_H

#include <simplestation.h>
#include <memory/memory.h>
#include <memory/dma/channel.h>
#include <gpu/gp0.h>
#include <ui/termcolour.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* Function Definitions */
uint8_t m_dma_init(m_simplestation_state *m_simplestation);

void m_dma_write(uint32_t m_addr, uint32_t m_value, m_simplestation_state *m_simplestation);
uint32_t m_dma_read(uint32_t m_addr, m_simplestation_state *m_simplestation);

void m_set_interrupt(uint32_t m_val, m_simplestation_state *m_simplestation);
bool m_irq(m_simplestation_state *m_simplestation);
uint32_t m_get_interrupt(m_simplestation_state *m_simplestation);
void dma_step(m_simplestation_state *m_simplestation);
void m_dma_run(m_simplestation_state *m_simplestation, uint8_t m_id);
void m_dma_run_block(m_simplestation_state *m_simplestation, uint8_t m_id);
void m_dma_run_linked_list(m_simplestation_state *m_simplestation, uint8_t m_id);

void m_dma_exit(m_simplestation_state *m_simplestation);

#endif /* DMA_H */
