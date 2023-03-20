#include <simplestation.h>

#define PARAM_FIFO_SZ   16

/* Function Definitions */
void m_cdrom_parameter_fifo_init(m_simplestation_state *m_simplestation);
void m_cdrom_parameter_fifo_push(uint8_t m_parameter, m_simplestation_state *m_simplestation);
uint8_t m_cdrom_parameter_fifo_pop(m_simplestation_state *m_simplestation);
