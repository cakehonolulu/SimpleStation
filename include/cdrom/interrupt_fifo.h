#include <simplestation.h>

#define INTERRUPT_FIFO_SIZE 64

/* Function Definitions */
void m_cdrom_interrupt_fifo_init(m_simplestation_state *m_simplestation);
uint8_t m_cdrom_interrupt_fifo_size(m_simplestation_state *m_simplestation);
uint8_t m_cdrom_interrupt_fifo_is_empty(m_simplestation_state *m_simplestation);
uint8_t m_cdrom_interrupt_fifo_front(m_simplestation_state *m_simplestation);
void m_cdrom_interrupt_fifo_push(uint8_t m_parameter, m_simplestation_state *m_simplestation);
uint8_t m_cdrom_interrupt_fifo_pop(m_simplestation_state *m_simplestation);
void m_interrupt_fifo_flush(m_simplestation_state *m_simplestation);
