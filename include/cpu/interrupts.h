#include <ui/termcolour.h>
#include <simplestation.h>
#include <stdint.h>
#include <stdio.h>

/* Defines */

// Interrupt Write Error Return Value
#define INTWERR 2

/* Function Definitions */
void m_interrupts_init();
uint32_t m_interrupts_write(uint32_t m_int_addr, uint32_t m_int_val);