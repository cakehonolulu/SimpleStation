#include <cpu/instructions.h>
#include <cpu/bios.h>
#include <ui/termcolour.h>
#include <debugger/debugger.h>

void m_exp(m_simplestation_state *m_simplestation)
{
	// Check if the instruction is implemented
	if (m_psx_extended_00[(m_simplestation->m_cpu->m_opcode & 0x3F)].m_funct == NULL)
	{
		printf(RED "[CPU] fde->exp: Unimplemented '0x00 Family' Opcode: 0x%02X (Full Opcode: 0x%4X)\n" NORMAL, (m_simplestation->m_cpu->m_opcode & 0x3F), m_simplestation->m_cpu->m_opcode);
		m_printregs(m_simplestation);
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		// Execute the instruction
		((void (*)(m_simplestation_state *m_simplestation))m_psx_extended_00[(m_simplestation->m_cpu->m_opcode & 0x3F)].m_funct)(m_simplestation);
	}
}

void m_cop0(m_simplestation_state *m_simplestation)
{
	// Check if the instruction is implemented
	if (m_psx_cop0[REGIDX_S].m_funct == NULL)
	{
		printf(RED "[CPU] fde->cop0: Unimplemented 'Coprocessor 0' Opcode: 0x%02X (Full Opcode: 0x%4X)\n" NORMAL, REGIDX_S, m_simplestation->m_cpu->m_opcode);
		m_printregs(m_simplestation);
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		// Execute the instruction
		((void (*)(void))m_psx_cop0[REGIDX_S].m_funct)();
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
void m_sll(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sll $%s, $%s, %x\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
#endif

	REGS[REGIDX_D] = REGS[REGIDX_T] << SHIFT;
}

/*
	ADDU (MIPS I)
	
	Format:
	ADDU rd, rs, rt 

	Description:
	To add 32-bit integers.
	The 32-bit word value in GPR rt is added to the 32-bit value in GPR rs and the 32-bit
	arithmetic result is placed into GPR rd .
	No Integer Overflow exception occurs under any circumstances.
*/
void m_addu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("addu $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	REGS[REGIDX_D] = REGS[REGIDX_S] + REGS[REGIDX_T];
}

/*
	OR (MIPS I)

	Format:
	OR rd, rs, rt

	Description:
	To do a bitwise logical OR.
	The contents of GPR rs are combined with the contents of GPR rt in a bitwise logical
	OR operation. The result is placed into GPR rd.
*/
void m_or(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("or $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	REGS[REGIDX_D] = (REGS[REGIDX_S] | REGS[REGIDX_T]);
}

/*
	SLTU (MIPS I)

	Format:
	SLTU rd, rs, rt

	Description:
	To record the result of an unsigned less-than comparison.
	Compare the contents of GPR rs and GPR rt as unsigned integers and record the
	Boolean result of the comparison in GPR rd. If GPR rs is less than GPR rt the result is
	1 (true), otherwise 0 (false).
	The arithmetic comparison does not cause an Integer Overflow exception.
*/
void m_sltu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sltu $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	if (REGS[REGIDX_S] < REGS[REGIDX_T])
	{
		REGS[REGIDX_D] = 1;
	}
	else
	{
		REGS[REGIDX_D] = 0;
	}
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
void m_j(m_simplestation_state *m_simplestation)
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
	BNE (MIPS I)

	Format:
	BNE rs, rt, offset

	Description:
	To compare GPRs then do a PC-relative conditional branch.
	An 18-bit signed offset (the 16-bit offset field shifted left 2 bits) is added to the address
	of the instruction following the branch (not the branch itself), in the branch delay slot,
	to form a PC-relative effective target address.
	If the contents of GPR rs and GPR rt are not equal, branch to the effective target address
	after the instruction in the delay slot is executed.
*/
void m_bne(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("bne $%s, $%s, %d\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T], (SIMMDT) << 2);
#endif
	
	/*
		According to simias, we only need the 16-bit sign-extended immediate,
		but MIPS Reference Manual specifies that we need to bit-shift it to the
		left by 2 times to get a 18-bit relative address
	*/
	if (REGS[REGIDX_S] != REGS[REGIDX_T])
	{
		PC += ((SIMMDT) << 2);
		PC -= 4;
	}
}

/*
	ADDI (MIPS I)

	Format:
	ADDI rt, rs, immediate

	Description:
	To add a constant to a 32-bit integer. If overflow occurs, then trap.
	The 16-bit signed immediate is added to the 32-bit value in GPR rs to produce a 32-bit
	result. If the addition results in 32-bit 2’s complement arithmetic overflow then the
	destination register is not modified and an Integer Overflow exception occurs. If it
	does not overflow, the 32-bit result is placed into GPR rt
*/
void m_addi(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("addi $%s, $%s, %d\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T], SIMMDT);
#endif

	if (m_cpu_check_signed_addition(REGS[REGIDX_S], SIMMDT))
	{
		printf(RED "[CPU] addi: Integer overflow! Panicking...\n");
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		REGS[REGIDX_T] = REGS[REGIDX_S] + SIMMDT;
	}
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
void m_addiu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("addiu $%x, $%s, 0x%X\n", REGIDX_T, m_cpu_regnames[REGIDX_S], SIMMDT);
#endif

	REGS[REGIDX_T] = REGS[REGIDX_S] + SIMMDT;
}

/*
	LW (MIPS I)

	Format:
	LW rt, offset(base)

	Description:
	To load a word from memory as a signed value
	The contents of the 32-bit word at the memory location specified by the aligned
	effective address are fetched, sign-extended to the GPR register length if necessary, and
	placed in GPR rt. The 16-bit signed offset is added to the contents of GPR base to form
	the effective address.
*/
void m_lw(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lw $%s, 0x%x($%x)\n", m_cpu_regnames[REGIDX_T], IMMDT, REGIDX_S);
#endif

	if ((COP0_STATUS_REGISTER & 0x10000) != 0)
	{
		printf(YELLOW "[CPU] lw: Ignoring word store, cache is isolated...\n" NORMAL);
		return;
	}
	
	int32_t m_addr = (SIMMDT + REGS[REGIDX_S]);
	REGS[REGIDX_T] = m_memory_read(m_addr, dword, m_simplestation);
}

/*
	SH (MIPS I)

	Format:
	SH rt, offset(base)

	Description:
	To store a halfword to memory.
	The least-significant 16-bit halfword of register rt is stored in memory at the location
	specified by the aligned effective address. The 16-bit signed offset is added to the
	contents of GPR base to form the effective address.
*/
void m_sh(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sh $%s, 0x%x($%s)\n", m_cpu_regnames[REGIDX_T], IMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	if ((COP0_STATUS_REGISTER & 0x10000) != 0)
	{
		printf(YELLOW "[CPU] sh: Ignoring word store, cache is isolated...\n" NORMAL);
		return;
	}
	
	m_memory_write(REGS[REGIDX_S] + SIMMDT, REGS[REGIDX_T], word, m_simplestation);
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
void m_sw(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sw $%s, 0x%X($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	if ((COP0_STATUS_REGISTER & 0x10000) != 0)
	{
#ifdef DEBUG_INSTRUCTIONS
		printf(YELLOW "[CPU] sw: Ignoring word store, cache is isolated...\n" NORMAL);
#endif
		return;
	}

	m_memory_write((REGS[REGIDX_S] + SIMMDT), REGS[REGIDX_T], dword, m_simplestation);
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
void m_ori(m_simplestation_state *m_simplestation)
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
void m_lui(m_simplestation_state *m_simplestation)
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