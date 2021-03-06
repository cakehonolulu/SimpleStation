#include <cpu/interrupts.h>
#include <stdlib.h>

void m_interrupts_init(m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cpu_ints = (m_psx_cpu_ints_t *) malloc(sizeof(m_psx_cpu_ints_t));

	m_simplestation->m_cpu_ints->m_interrupt_stat = 0;
	m_simplestation->m_cpu_ints->m_interrupt_mask = 0;

	m_simplestation->m_interrupts_state = ON;
}

uint32_t m_interrupts_write(uint32_t m_int_addr, uint32_t m_int_val, m_simplestation_state *m_simplestation)
{
	uint32_t m_reg = m_int_addr & 0xF;

	if (!m_reg)
	{
		return m_simplestation->m_cpu_ints->m_interrupt_stat &= m_int_val & 0x7FF;
	}
	else if (m_reg == 4)
	{
		return m_simplestation->m_cpu_ints->m_interrupt_mask = m_int_val & 0x7FF;
	}

	 printf(RED "[INT] write: Abnormal path in emulator code, continuing might break things; exiting...\n" NORMAL);
	 return m_simplestation_exit(m_simplestation, 1);
}