#include <cpu/interrupts.h>

uint32_t m_interrupt_stat;
uint32_t m_interrupt_mask;

void m_interrupts_init()
{
	m_interrupt_stat = 0;
	m_interrupt_mask = 0;

	m_simplestation.m_interrupts_state = ON;
}

uint32_t m_interrupts_write(uint32_t m_int_addr, uint32_t m_int_val)
{
	uint32_t m_reg = m_int_addr & 0xF;

	if (!m_reg)
	{
		return m_interrupt_stat &= m_int_val & 0x7FF;
	}
	else if (m_reg == 4)
	{
		return m_interrupt_mask = m_int_val & 0x7FF;
	}

	 printf(RED "[INT] write: Abnormal path in emulator code, continuing might break things; exiting...\n" NORMAL);
	 return m_simplestation_exit(1);
}