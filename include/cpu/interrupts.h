#include <ui/termcolour.h>
#include <simplestation.h>
#include <stdint.h>
#include <stdio.h>

/* Defines */

// Interrupt Write Error Return Value
#define INTWERR 2

#define COP0_CAUSE_IP  (COP0_CAUSE & 0x0000FF00)
#define COP0_SR_IM  (COP0_SR & 0x0000FF00)

/* Function Definitions */
uint8_t m_interrupts_init(m_simplestation_state *m_simplestation);
void m_interrupts_write(uint32_t m_int_addr, uint32_t m_int_val, m_simplestation_state *m_simplestation);
uint32_t m_interrupts_read(uint32_t m_int_addr, m_simplestation_state *m_simplestation);
void m_interrupts_check(m_simplestation_state *m_simplestation);
void m_interrupts_request(m_int_types m_int, m_simplestation_state *m_simplestation);
void m_interrupts_exit(m_simplestation_state *m_simplestation);
