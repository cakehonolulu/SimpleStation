#include <simplestation.h>

#define m_cdrom_response_fifo_front (m_simplestation->m_cdrom->m_response_fifo[0])

/* Function Definitions */
void m_cdrom_response_fifo_init(m_simplestation_state *m_simplestation);
void m_cdrom_response_fifo_push(uint8_t m_response, m_simplestation_state *m_simplestation);
uint8_t m_cdrom_response_fifo_pop(m_simplestation_state *m_simplestation);
