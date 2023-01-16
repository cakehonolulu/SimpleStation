#include <simplestation.h>

/* Function Definitions */
void m_cdrom_response_fifo_init(m_simplestation_state *m_simplestation);
void m_cdrom_response_fifo_push(uint8_t m_response, m_simplestation_state *m_simplestation);
uint8_t m_cdrom_response_fifo_pop(m_simplestation_state *m_simplestation);
