#include <ui/termcolour.h>
#include <simplestation.h>
#include <stdint.h>
#include <stdio.h>

/* Defines */

// Interrupt Write Error Return Value
#define INTWERR 2

/* Function Definitions */
uint8_t m_interrupts_init(m_simplestation_state *m_simplestation);
void m_interrupts_trigger(uint32_t irq, m_simplestation_state *m_simplestation);
bool m_interrupts_check(m_simplestation_state *m_simplestation);
void m_interrupts_update(m_simplestation_state *m_simplestation);
void m_interrupts_write(uint32_t m_int_addr, uint32_t m_int_val, m_simplestation_state *m_simplestation);
uint32_t m_interrupts_read(uint32_t m_int_addr, m_simplestation_state *m_simplestation);
void m_interrupts_exit(m_simplestation_state *m_simplestation);
