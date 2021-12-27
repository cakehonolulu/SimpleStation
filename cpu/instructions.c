#include <cpu/instructions.h>

const struct m_corewave_cw33300_instrs m_psx_instrs[67] = {
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{"lui", m_lui},	// 0x0F
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL}	// 0x00
};

void m_lui()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lui $%s, 0x%X\n", m_cpu_regnames[REGVAL], IMMDT);
#endif

	// Check if register isn't register zero
	if (m_regidx)
	{
		REGS[REGVAL] = (IMMDT << 16);
	}
	
	PC += 4;
}