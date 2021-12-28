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

/*
	LUI (MIPS I)
	
	Format: LUI rt, immediate 
	
	Description:
	Load a constant into the upper half of a word.
	The 16-bit immediate is shifted left 16 bits and concatenated with 16 bits of low-order
	zeros. The 32-bit result is sign-extended and placed into GPR rt.
*/
void m_lui()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lui $%s, 0x%X\n", m_cpu_regnames[REGVAL], IMMDT);
#endif

	// Check if register isn't register zero
	if (m_regidx)
	{
		// Bit shift by 16 the immediate value and place it at the register pointed by the index
		REGS[REGVAL] = (IMMDT << 16);
	}
	
	// Increment Program Counter by 4
	PC += 4;
}