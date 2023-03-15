#pragma once

#include <simplestation.h>

/* Function Definitions */
uint8_t gte_init(m_simplestation_state *m_simplestation);
uint8_t gte_exit(m_simplestation_state *m_simplestation);
void gte_execute(uint32_t opcode, m_simplestation_state *m_simplestation);
