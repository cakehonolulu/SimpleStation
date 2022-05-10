#include <cpu/cop0.h>

void m_cpu_cop0_init(m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cpu_cop0 = (m_mips_r3000a_cop0_t*) malloc(sizeof(m_mips_r3000a_cop0_t));

	if (!m_simplestation->m_cpu_cop0)
	{
		printf("Simplestation: Couldn't allocate CPU's Coprocessor 0 state struct, exiting...\n");
		exit(EXIT_FAILURE);
	}
#ifdef DEBUG_CPU_COP0
	else
	{
		printf("[CPU] Allocated CPU's Coprocessor 0 structure!\n");
	}
#endif

	// Set all Coprocessor 0 Registers to 0
	for (uint8_t m_cop0_regs = 0; m_cop0_regs < M_R3000_COP0_REGISTERS; m_cop0_regs++)
	{
		m_simplestation->m_cpu_cop0->m_registers[m_cop0_regs] = 0;
	}
}

void m_cpu_cop0_exit(m_simplestation_state *m_simplestation)
{
	free(m_simplestation->m_cpu_cop0);
}

void m_mtc0(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("mtc0 $%s, $%s\n", m_cpu_regnames[REGIDX_T], m_cop0_regnames[REGIDX_D]);
#endif

	COP0_REGS[REGIDX_D] = REGS[REGIDX_T];
}