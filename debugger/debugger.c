#include <debugger/debugger.h>
#include <simplestation.h>

void m_printregs(m_simplestation_state *m_simplestation)
{
	printf(MAGENTA UNDERSCORE "\nGeneral Prupose Registers:\n" WHITE NORMAL);

	for (size_t m_idx = 0; m_idx < M_R3000_REGISTERS; m_idx++)
	{
		printf(MAGENTA "%s:" NORMAL WHITE " 0x%08X ", m_cpu_regnames[m_idx], m_simplestation->m_cpu->m_registers[m_idx]);
		
		if (m_idx % 2)
		{
			printf("\n");
		}
	}

	printf(BLUE UNDERSCORE "\nControl-Flow Registers:\n" WHITE NORMAL);
	printf(BLUE "PC:" WHITE " 0x%08X\n", PC);

	printf(GREEN UNDERSCORE "\nArithmetic Registers:\n" WHITE NORMAL);
	printf(GREEN "HI:" WHITE " 0x%08X\n", HI);
	printf(GREEN "LO:" WHITE " 0x%08X\n", LO);

	printf(YELLOW UNDERSCORE "\nSegment Registers:\n" WHITE NORMAL);
}