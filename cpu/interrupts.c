#include <cpu/interrupts.h>
#include <cpu/cpu.h>
#include <stdlib.h>

uint8_t m_interrupts_init(m_simplestation_state *m_simplestation)
{
	uint8_t m_return = 0;

	m_simplestation->m_cpu_ints = (m_psx_cpu_ints_t *) malloc(sizeof(m_psx_cpu_ints_t));

	if (m_simplestation->m_cpu_ints)
	{
		m_simplestation->m_cpu_ints->m_interrupt_stat = 0;
		m_simplestation->m_cpu_ints->m_interrupt_mask = 0;

		m_simplestation->m_interrupts_state = ON;
	}
	else
	{
		printf("[INT] init: Couldn't initialize PSX's Interrupts, exiting...");
		m_return = 1;
	}

	return m_return;
}

bool m_interrupt_isactive(m_simplestation_state *m_simplestation)
{
    return (m_simplestation->m_cpu_ints->m_interrupt_stat & m_simplestation->m_cpu_ints->m_interrupt_mask) != 0;
}

void m_interrupts_request(m_int_types m_int, m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cpu_ints->m_interrupt_stat |= 1 << (uint32_t) m_int;
	m_interrupts_update(m_simplestation);
}

bool m_interrupts_pending(m_simplestation_state *m_simplestation)
{
	return (COP0_CAUSE_IP & COP0_SR_IM) && (COP0_SR & 1);
}

void m_interrupts_update(m_simplestation_state *m_simplestation)
{
	COP0_CAUSE = (COP0_CAUSE & 0xFFFF00FF) | ((uint32_t)(m_interrupt_isactive(m_simplestation) ? 4 : 0) <<  8);
}

void m_interrupts_write(uint32_t m_int_addr, uint32_t m_int_val, m_simplestation_state *m_simplestation)
{
	switch (m_int_addr)
	{
		case 0:
			m_simplestation->m_cpu_ints->m_interrupt_stat &= m_int_val;
			break;

		case 4:
			m_simplestation->m_cpu_ints->m_interrupt_mask = m_int_val;
			break;
	
		default:
			printf(RED "[INT] write: Unhandled Interrupts Write (Offset: %d, Value: 0x%08X)\n" NORMAL, m_int_addr, m_int_val);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}

uint32_t m_interrupts_read(uint32_t m_int_addr, m_simplestation_state *m_simplestation)
{
	uint32_t m_return;

	switch (m_int_addr)
	{
		case 0:
			m_return = m_simplestation->m_cpu_ints->m_interrupt_stat;
			break;

		case 4:
			m_return = m_simplestation->m_cpu_ints->m_interrupt_mask;
			break;
	
		default:
			printf(RED "[INT] read: Unhandled Interrupts Read (Offset: %d)\n" NORMAL, m_int_addr);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}

	return m_return;
}

void m_interrupts_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_interrupts_state)
    {
        free(m_simplestation->m_cpu_ints);
    }

    m_simplestation->m_gpu_state = OFF;
}
