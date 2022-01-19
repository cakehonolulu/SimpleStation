#include <cpu/instructions.h>

const struct m_corewave_cw33300_instrs m_psx_instrs[67] = {
	{"sll", m_sll},		// 0x00
	{NULL, NULL},		// 0x01
	{"j", m_j},			// 0x02
	{NULL, NULL},		// 0x03
	{NULL, NULL},		// 0x04
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{"addiu", m_addiu},	// 0x09
	{NULL, NULL},		// 0x00
	{"sw", m_sw},		// 0x0B
	{NULL, NULL},		// 0x00
	{"ori", m_ori},		// 0x0D
	{NULL, NULL},		// 0x00
	{"lui", m_lui},		// 0x0F
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x00
	{NULL, NULL}		// 0x00
};

void m_sll()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sll $%s, $%s, %x\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
#endif

	REGS[REGIDX_D] = REGS[REGIDX_T] << SHIFT;
}

void m_j()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("PC: 0x%x; dst: 0x%x\n", PC, ((PC & 0xFC000000) | ((m_opcode & 0x3FFFFFF) * 4)));
#endif

	PC = ((PC & 0xFC000000) | ((m_opcode & 0x3FFFFFF) * 4));
}

void m_addiu()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("addiu $%x, $%s, 0x%X\n", REGIDX_T, m_cpu_regnames[REGIDX_S], SIMMDT);
#endif

	REGS[REGIDX_T] = REGS[REGIDX_S] + SIMMDT;
}

void m_sw()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sw $%s, 0x%X($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	m_memory_write((REGS[REGIDX_S] + SIMMDT), REGS[REGIDX_T], dword);
}

/*
	ORI (MIPS I)

	Format:
	ORI rt, rs, immediate

	Description:
	To do a bitwise logical OR with a constant.
	The 16-bit immediate is zero-extended to the left and combined with the contents of
	GPR rs in a bitwise logical OR operation. The result is placed into GPR rt. 
*/
void m_ori()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("ori $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], IMMDT);
#endif

	// Check if register isn't register zero
	if (REGIDX_T)
	{
		REGS[REGIDX_T] = (REGS[REGIDX_S] | IMMDT);
	}
}

/*
	LUI (MIPS I)
	
	Format:
	LUI rt, immediate 
	
	Description:
	Load a constant into the upper half of a word.
	The 16-bit immediate is shifted left 16 bits and concatenated with 16 bits of low-order
	zeros. The 32-bit result is sign-extended and placed into GPR rt.
*/
void m_lui()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lui $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], IMMDT);
#endif

	// Check if recieving register isn't register zero
	if (REGIDX_T)
	{
		// Bit shift by 16 the immediate value and place it at the register pointed by the index
		REGS[REGIDX_T] = (IMMDT << 16);
	}
}