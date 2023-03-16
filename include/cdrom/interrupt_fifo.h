#include <simplestation.h>

/* Function Definitions */
void m_cdrom_interrupt_fifo_init(m_simplestation_state *m_simplestation);
void m_cdrom_interrupt_fifo_push(uint8_t m_interrupt, m_simplestation_state *m_simplestation);
void m_interrupt_fifo_flush(m_simplestation_state *m_simplestation);
uint8_t m_cdrom_interrupt_fifo_pop(m_simplestation_state *m_simplestation);
