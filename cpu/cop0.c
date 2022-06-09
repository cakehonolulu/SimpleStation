#include <cpu/cop0.h>

uint8_t m_cpu_cop0_init(m_simplestation_state *m_simplestation)
{
	uint8_t m_return = 0;

	// Malloc the COP0-state struct
	m_simplestation->m_cpu_cop0 = (m_mips_r3000a_cop0_t*) malloc(sizeof(m_mips_r3000a_cop0_t));

	// Check if it has been allocated
	if (m_simplestation->m_cpu_cop0)
	{
#ifdef DEBUG_CPU_COP0
		printf("[CPU] Allocated CPU's Coprocessor 0 structure!\n");
#endif

		// Set all Coprocessor 0 Registers to 0
		for (uint8_t m_cop0_regs = 0; m_cop0_regs < M_R3000_COP0_REGISTERS; m_cop0_regs++)
		{
			m_simplestation->m_cpu_cop0->m_registers[m_cop0_regs] = 0;
		}
	}
	else
	{
		printf("Simplestation: Couldn't allocate CPU's Coprocessor 0 state struct, exiting...\n");
		m_return = 1;
	}

	return m_return;
}

void m_cpu_cop0_exit(m_simplestation_state *m_simplestation)
{
	// Free the COP0 struct if-and-only-if has been allocated
	if (m_simplestation->m_cpu_cop0 != NULL)
	{
		free(m_simplestation->m_cpu_cop0);	
	}
}

void m_mtc0(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("mtc0 $%s, $%s\n", m_cpu_regnames[REGIDX_T], m_cop0_regnames[REGIDX_D]);
#endif

	COP0_REGS[REGIDX_D] = REGS[REGIDX_T];
}
