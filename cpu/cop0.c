#include <cpu/cop0.h>

m_mips_r3000a_cop0_t *m_cpu_cop0;

void m_cpu_cop0_init()
{
	m_cpu_cop0 = (m_mips_r3000a_cop0_t*) malloc(sizeof(m_mips_r3000a_cop0_t));

	if (!m_cpu_cop0)
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
}

void m_cpu_cop0_exit()
{
	free(m_cpu_cop0);
}

void m_mtc0()
{
	printf("!\n");
	exit(1);
}