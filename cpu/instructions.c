#include <cpu/instructions.h>

void m_exp()
{
	// Check if the instruction is implemented
	if (m_psx_extended_00[(m_opcode & 0x3F)].m_funct == NULL)
	{
		printf("Unimplemented 0x00 Family Opcode: 0x%02X\n", (m_opcode & 0x3F));
		m_printregs();
		m_bios_exit();
		m_cpu_exit();
		exit(EXIT_FAILURE);
	}
	else
	{
		// Execute the instruction
		((void (*)(void))m_psx_extended_00[(m_opcode & 0x3F)].m_funct)();
	}
}

/*
	SLL (MIPS I)

	Format:
	SLL rd, rt, sa

	Description:
	To left shift a word by a fixed number of bits.
	The contents of the low-order 32-bit word of GPR rt are shifted left, inserting zeroes
	into the emptied bits; the word result is placed in GPR rd. The bit shift count is
	specified by sa. If rd is a 64-bit register, the result word is sign-extended.
*/
void m_sll()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sll $%s, $%s, %x\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
#endif

	REGS[REGIDX_D] = REGS[REGIDX_T] << SHIFT;
}

/*
	J (MIPS I)

	Format:
	J target

	Description:
	To branch within the current 256 MB aligned region
	This is a PC-region branch (not PC-relative); the effective target address is in the
	“current” 256 MB aligned region. The low 28 bits of the target address is the instr_index
	field shifted left 2 bits. The remaining upper bits are the corresponding bits of the
	address of the instruction in the delay slot (not the branch itself).
	Jump to the effective target address. Execute the instruction following the jump, in the
	branch delay slot, before jumping
*/
void m_j()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("j 0x%x\n", ((PC & 0xF0000000) | (JIMMDT * 4)));
#endif

	/*
		According to simias, the immediate value is shifted 2 times to the right,
		but psx spx wiki specifies the immediate jump value is multiplied by 4
	*/
	PC = ((PC & 0xF0000000) | (JIMMDT * 4));
}

/*
	ADDIU (MIPS I)

	Format:
	ADDIU rt, rs, immediate

	Description:
	The 16-bit signed immediate is added to the 32-bit value in GPR rs and the 32-bit
	arithmetic result is placed into GPR rt.
	No Integer Overflow exception occurs under any circumstances.
*/
void m_addiu()
{
#ifdef DEBUG_INSTRUCTIONS
	printf("addiu $%x, $%s, 0x%X\n", REGIDX_T, m_cpu_regnames[REGIDX_S], SIMMDT);
#endif

	REGS[REGIDX_T] = REGS[REGIDX_S] + SIMMDT;
}

/*
	SW (MIPS I)

	Format:
	SW rt, offset(base)

	Description:
	The least-significant 32-bit word of register rt is stored in memory at the location
	specified by the aligned effective address. The 16-bit signed offset is added to the
	contents of GPR base to form the effective address.
*/
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