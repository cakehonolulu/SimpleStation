#include <debugger/debugger.h>

void m_printregs()
{
	printf("Z0: %x    ", m_zero);
	
	for (size_t m_idx = 0; m_idx < (M_R3000_REGISTERS - 1); m_idx++)
	{
		printf("%s: %x    ", m_cpu_regnames[m_idx], m_cpu->m_registers[m_idx]);
		
		if (!(m_idx % 2))
		{
			printf("\n");
		}
	}

}