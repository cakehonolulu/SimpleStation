#include <cpu/instructions.h>
#include <cpu/bios.h>
#include <ui/termcolour.h>
#include <debugger/debugger.h>

void m_exp(m_simplestation_state *m_simplestation)
{
	// Check if the instruction is implemented
	if (m_psx_extended_00[(m_simplestation->m_cpu->m_opcode & 0x3F)].m_funct == NULL)
	{
		printf(RED "[CPU] fde->exp: Unimplemented '0x00 Family' Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, (m_simplestation->m_cpu->m_opcode & 0x3F), (uint32_t) m_simplestation->m_cpu->m_opcode);
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
		printf(RED "[CPU] fde->cop0: Unimplemented 'Coprocessor 0' Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, REGIDX_S, m_simplestation->m_cpu->m_opcode);
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		// Execute the instruction
		((void (*)(m_simplestation_state *m_simplestation))m_psx_cop0[REGIDX_S].m_funct)(m_simplestation);
	}
}

/*
	BXX
	Branch instructions
*/
void m_bxx(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	if ((m_simplestation->m_cpu->m_opcode >> 16) & 1)
	{
		if (((m_simplestation->m_cpu->m_opcode >> 17) & 0xF) == 8)
		{
			printf("bgezal $%s, %d\n", m_cpu_regnames[REGIDX_S], SIMMDT << 2);
		}
		else
		{
			printf("bgez $%s, %d\n", m_cpu_regnames[REGIDX_S], SIMMDT << 2);
		}
	}
	else
	{
		if (((m_simplestation->m_cpu->m_opcode >> 17) & 0xF) == 8)
		{
			printf("bltzal $%s, %d\n", m_cpu_regnames[REGIDX_S], SIMMDT << 2);
		}
		else
		{
			printf("bltz $%s, %d\n", m_cpu_regnames[REGIDX_S], SIMMDT << 2);
		}
	}
#endif

	bool m_bgez = (m_simplestation->m_cpu->m_opcode >> 16) & 1;
	bool m_link = ((m_simplestation->m_cpu->m_opcode >> 17) & 0xF) == 8;

	int32_t v = REGS[REGIDX_S];

	uint32_t m_test = (v < 0);

	m_test ^= m_bgez;

	if (m_link)
	{
		uint32_t m_ra = PC + 4;
		REGS[31] = m_ra;
	}

	if (m_test)
	{
		m_cpu_branch(SIMMDT, m_simplestation);
	}
}

// Exception handler
void m_exception(m_exc_types m_exception, m_simplestation_state *m_simplestation)
{
	// BEV bit in COP0's SR register decides where
	uint32_t m_dst = (((COP0_SR & (1 << 2)) != 0) ? 0xBFC00180 : 0x80000080);

	uint8_t m_mode = COP0_SR & 0x3F;
	COP0_SR &= ~0x3F;

	COP0_SR |= ((m_mode << 2) & 0x3F);

	COP0_CAUSE = (((uint32_t) m_exception) << 2);

	COP0_EPC = PC - 4;

	/* FIXME: Next 2 lines are suboptimal */
	NXT_PC = m_dst;
	m_simplestation->m_cpu->m_next_opcode = m_memory_read((NXT_PC), dword, m_simplestation);

	if (m_simplestation->m_cpu->m_branch)
	{
		COP0_EPC -= 4;
		COP0_CAUSE |= (1 << 31);
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
	if (m_simplestation->m_cpu->m_opcode == 0)
	{
		printf("nop\n");
	}
	else
	{
		printf("sll $%s, $%s, %x\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
	}
#endif

	REGS[REGIDX_D] = REGS[REGIDX_T] << SHIFT;
}

/*
	SRL (MIPS I)

	Format:
	SRL rd, rt, sa

	Description:
	The contents of the low-order 32-bit word of GPR rt are shifted right, inserting zeros
	into the emptied bits; the word result is placed in GPR rd. The bit shift count is
	specified by sa. If rd is a 64-bit register, the result word is sign-extended.
*/
void m_srl(m_simplestation_state *m_simplestation)
{
	#ifdef DEBUG_INSTRUCTIONS
	printf("srl $%s, $%s, %x\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
#endif

	REGS[REGIDX_D] = REGS[REGIDX_T] >> SHIFT;
}

/*
	SRA (MIPS I)
	Format:
	SRA rd, rt, sa
	Description:
	The contents of the low-order 32-bit word of GPR rt are shifted right, duplicating the
	sign-bit (bit 31) in the emptied bits; the word result is placed in GPR rd. The bit shift
	count is specified by sa. If rd is a 64-bit register, the result word is sign-extended.
*/
void m_sra(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sra $%s, $%s, %d\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
#endif

	REGS[REGIDX_D] = ((uint32_t) (((int32_t) (REGS[REGIDX_T])) >> SHIFT));
}

/*
	JR (MIPS I)

	Format:
	JR rs MIPS I

	Description:
	To branch to an instruction address in a register.
	Jump to the effective target address in GPR rs. Execute the instruction following the
	jump, in the branch delay slot, before jumping.
*/
void m_jr(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("jr $%s\n", m_cpu_regnames[REGIDX_S]);
#endif

	NXT_PC = REGS[REGIDX_S];
	m_simplestation->m_cpu->m_branch = true;
}

/*
	JALR (MIPS I)

	Format(s):
	JALR rs (rd = 31 implied)
	JALR rd, rs
	
	Description:
	Place the return address link in GPR rd. The return link is the address of the second
	instruction following the branch, where execution would continue after a procedure
	call.
	Jump to the effective target address in GPR rs. Execute the instruction following the
	jump, in the branch delay slot, before jumping.
*/
void m_jalr(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	if (REGIDX_D == 31)
	{
		printf("jalr $%s\n", m_cpu_regnames[REGIDX_S]);
	}
	else
	{
		printf("jalr $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S]);
	}
#endif

	REGS[REGIDX_D] = PC + 4;
	NXT_PC = REGS[REGIDX_S];
	m_simplestation->m_cpu->m_branch = true;
}

/*
	SYSCALL (MIPS I)

	Format:
	SYSCALL
	
	Description:
	A system call exception occurs, immediately and unconditionally transferring control
	to the exception handler.
	The code field is available for use as software parameters, but is retrieved by the
	exception handler only by loading the contents of the memory word containing the
	instruction.
*/
void m_syscall(m_simplestation_state *m_simplestation)
{
	m_exc_types m_exc = syscall;

	m_exception(m_exc, m_simplestation);
}

/*
	MFHI (MIPS I)

	Format:
	MFHI rd

	Description:
	The contents of special register HI are loaded into GPR rd.
*/
void m_mfhi(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("mfhi $%s\n", m_cpu_regnames[REGIDX_D]);
#endif

	REGS[REGIDX_D] = HI;
}

/*
	MTHI (MIPS I)

	Format:
	MTHI rs

	Description:
	The contents of GPR rs are loaded into special register HI.
*/
void m_mthi(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("mthi $%s\n", m_cpu_regnames[REGIDX_S]);
#endif

	HI = REGS[REGIDX_S];
}

/*
	MFLO (MIPS I)

	Format:
	MFLO rd

	Description:
	The contents of special register LO are loaded into GPR rd.
	Restrictions:
	The two instructions that follow an MFLO instruction must not be instructions that
	modify the LO register: DDIV, DDIVU, DIV, DIVU, DMULT, DMULTU, MTLO, MULT,
	MULTU. If this restriction is violated, the result of the MFLO is undefined.
*/
void m_mflo(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("mflo $%s\n", m_cpu_regnames[REGIDX_D]);
#endif

	REGS[REGIDX_D] = LO;
}

/*
	MTLO (MIPS I)

	Format:
	MTLO rs

	Description:
	The contents of GPR rs are loaded into special register LO.
*/
void m_mtlo(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("mtlo $%s\n", m_cpu_regnames[REGIDX_S]);
#endif

	LO = REGS[REGIDX_S];
}

/*
	DIV (MIPS I)

	Format:
	DIV rs, rt

	Description:
	The 32-bit word value in GPR rs is divided by the 32-bit value in GPR rt, treating both
	operands as signed values. The 32-bit quotient is placed into special register LO and
	the 32-bit remainder is placed into special register HI.
	No arithmetic exception occurs under any circumstances.
*/
void m_div(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("div $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	if ((int32_t) (REGS[REGIDX_T]) == 0)
	{
		HI = (uint32_t) (REGS[REGIDX_S]);

		if ((int32_t) (REGS[REGIDX_S]) >= 0)
		{
			LO = 0xFFFFFFFF;
		}
		else
		{
			LO = 1;
		}
	}
	else if (((uint32_t) (REGS[REGIDX_S]) == 0x80000000) && ((int32_t) (REGS[REGIDX_T]) == -1))
	{
		HI = 0;
		LO = 0x80000000;
	}
	else
	{
		HI = ((uint32_t) (((int32_t) REGS[REGIDX_S]) % ((int32_t) REGS[REGIDX_T])));
		LO = ((uint32_t) (((int32_t) REGS[REGIDX_S]) / ((int32_t) REGS[REGIDX_T])));
	}
}

/*
	DIVU (MIPS I)

	Format:
	DIVU rs, rt

	Description:
	The 32-bit word value in GPR rs is divided by the 32-bit value in GPR rt, treating both
	operands as unsigned values. The 32-bit quotient is placed into special register LO and
	the 32-bit remainder is placed into special register HI.
	No arithmetic exception occurs under any circumstances.
*/
void m_divu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("divu $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	if (REGS[REGIDX_T] == 0)
	{
		HI = REGS[REGIDX_S];
		LO = 0xFFFFFFFF;
	}
	else
	{
		HI = REGS[REGIDX_S] % REGS[REGIDX_T];
		LO = REGS[REGIDX_S] / REGS[REGIDX_T];
	}
}

/*
 	ADD (MIPS I)

	Format:
	ADD rd, rs, rt
	
	Description:
	To add 32-bit integers. If overflow occurs, then trap.
	The 32-bit word value in GPR rt is added to the 32-bit value in GPR rs to produce a
	32-bit result. If the addition results in 32-bit 2’s complement arithmetic overflow then
	the destination register is not modified and an Integer Overflow exception occurs. If it
	does not overflow, the 32-bit result is placed into GPR rd .
*/
void m_add(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("add $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	if (m_cpu_check_signed_addition(REGS[REGIDX_S], REGS[REGIDX_T]))
	{
		printf(RED "[CPU] add: Integer overflow! Panicking...\n");
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		REGS[REGIDX_D] = ((int32_t) (((int32_t) REGS[REGIDX_S]) + ((int32_t) REGS[REGIDX_T])));
	}
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

	REGS[REGIDX_D] = (REGS[REGIDX_S] + REGS[REGIDX_T]);
}

/*
	SUBU (MIPS I)
	Format:
	SUBU rd, rs, rt
	Description:
	To subtract 32-bit integers.
	The 32-bit word value in GPR rt is subtracted from the 32-bit value in GPR rs and the
	32-bit arithmetic result is placed into GPR rd.
	No integer overflow exception occurs under any circumstances.
*/
void m_subu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("subu $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	REGS[REGIDX_D] = (REGS[REGIDX_S] - REGS[REGIDX_T]);
}

/*
	AND (MIPS I)
	
	Format:
	AND rd, rs, rt 

	Description:
	To do a bitwise logical AND.
	The contents of GPR rs are combined with the contents of GPR rt in a bitwise logical
	AND operation. The result is placed into GPR rd 
*/
void m_and(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("and $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	REGS[REGIDX_D] = (REGS[REGIDX_S] & REGS[REGIDX_T]);
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
	SLT (MIPS I)

	Format:
	SLT rd, rs, rt
	
	Description:
	Compare the contents of GPR rs and GPR rt as signed integers and record the Boolean
	result of the comparison in GPR rd. If GPR rs is less than GPR rt the result is 1 (true),
	otherwise 0 (false).
	The arithmetic comparison does not cause an Integer Overflow exception.
*/
void m_slt(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("slt $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	bool m_test = (((int32_t) REGS[REGIDX_S]) < ((int32_t) REGS[REGIDX_T]));

	REGS[REGIDX_D] = ((uint32_t) m_test);
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

	bool m_test = REGS[REGIDX_S] < REGS[REGIDX_T];

	REGS[REGIDX_D] = (uint32_t) m_test;
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
	NXT_PC = ((PC & 0xF0000000) | (JIMMDT * 4));
	m_simplestation->m_cpu->m_branch = true;
}

/*
	JAL (MIPS I)

	Format:
	J target

	Description:
	Branches and copies current PC to $ra ($31) for it to act as a return address (Similar to armv7 bl).
*/
void m_jal(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("jal 0x%x\n", ((PC & 0xF0000000) | (JIMMDT * 4)));
#endif

	REGS[31] = PC + 4;

	NXT_PC = ((PC & 0xF0000000) | (JIMMDT * 4));
	m_simplestation->m_cpu->m_branch = true;
}

/*
	BEQ (MIPS I)

	Format:
	BEQ rs, rt, offset

	Description:
	To compare GPRs then do a PC-relative conditional branch.
	An 18-bit signed offset (the 16-bit offset field shifted left 2 bits) is added to the address
	of the instruction following the branch (not the branch itself), in the branch delay slot,
	to form a PC-relative effective target address.
	If the contents of GPR rs and GPR rt are equal, branch to the effective target address
	after the instruction in the delay slot is executed.
*/
void m_beq(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("beq $%s, $%s, %d\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T], (SIMMDT) << 2);
#endif
	
	/*
		According to simias, we only need the 16-bit sign-extended immediate,
		but MIPS Reference Manual specifies that we need to bit-shift it to the
		left by 2 times to get a 18-bit relative address
	*/
	if (REGS[REGIDX_S] == REGS[REGIDX_T])
	{
		m_cpu_branch(SIMMDT, m_simplestation);
	}
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
		m_cpu_branch(SIMMDT, m_simplestation);
	}
}

/*
	BLEZ (MIPS I)
	Format:
	BLEZ rs, offset
	Description:
	To test a GPR then do a PC-relative conditional branch.ç
	An 18-bit signed offset (the 16-bit offset field shifted left 2 bits) is added to the address
	of the instruction following the branch (not the branch itself), in the branch delay slot,
	to form a PC-relative effective target address.
	If the contents of GPR rs are less than or equal to zero (sign bit is 1 or value is zero),
	branch to the effective target address after the instruction in the delay slot is executed.
*/
void m_blez(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("blez $%s, %d\n", m_cpu_regnames[REGIDX_S], (SIMMDT) << 2);
#endif

	int32_t m_val = REGS[REGIDX_S];

	if (m_val <= 0)
	{
		m_cpu_branch(SIMMDT, m_simplestation);
	}
}

/*
	BGTZ (MIPS I)
	Format:
	BGTZ rs, offset
	Description:
	To test a GPR then do a PC-relative conditional branch.
	An 18-bit signed offset (the 16-bit offset field shifted left 2 bits) is added to the address
	of the instruction following the branch (not the branch itself), in the branch delay slot,
	to form a PC-relative effective target address.
	If the contents of GPR rs are greater than zero (sign bit is 0 but value not zero), branch
	to the effective target address after the instruction in the delay slot is executed.
*/
void m_bgtz(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("bgtz $%s, %d\n", m_cpu_regnames[REGIDX_S], (SIMMDT) << 2);
#endif

	int32_t m_val = REGS[REGIDX_S];

	if (m_val > 0)
	{
		m_cpu_branch(SIMMDT, m_simplestation);
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
		REGS[REGIDX_T] = ((uint32_t) ( ((int32_t) REGS[REGIDX_S]) + ( (int32_t) SIMMDT)));
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
	printf("addiu $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], SIMMDT);
#endif

	REGS[REGIDX_T] = (REGS[REGIDX_S] + SIMMDT);
}

/*
	SLTI (MIPS I)

	Format:
	SLTI rt, rs, immediate

	Description:
	Compare the contents of GPR rs and the 16-bit signed immediate as signed integers and
	record the Boolean result of the comparison in GPR rt. If GPR rs is less than immediate
	the result is 1 (true), otherwise 0 (false).
	The arithmetic comparison does not cause an Integer Overflow exception.
*/
void m_slti(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("slti $%s, $%s, %d\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], SIMMDT);
#endif

	REGS[REGIDX_T] = ((int32_t) REGIDX_S) < ((int32_t) SIMMDT);
}

/*
	SLTIU (MIPS I)

	Format:
	SLTIU rt, rs, immediate

	Description:
	Compare the contents of GPR rs and the sign-extended 16-bit immediate as unsigned
	integers and record the Boolean result of the comparison in GPR rt. If GPR rs is less
	than immediate the result is 1 (true), otherwise 0 (false).
	Because the 16-bit immediate is sign-extended before comparison, the instruction is able
	to represent the smallest or largest unsigned numbers. The representable values are at
	the minimum [0, 32767] or maximum [max_unsigned-32767, max_unsigned] end of
	the unsigned range.
	The arithmetic comparison does not cause an Integer Overflow exception.
*/
void m_sltiu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sltiu $%s, $%s, %d\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], SIMMDT);
#endif

	REGS[REGIDX_T] = ((uint32_t) ((uint32_t) REGS[REGIDX_S]) < ((uint32_t) SIMMDT));
}

/*
	LB (MIPS I)

	Format:
	LB rt, offset(base)

	Description:
	To load a byte from memory as a signed value.
	The contents of the 8-bit byte at the memory location specified by the effective address
	are fetched, sign-extended, and placed in GPR rt. The 16-bit signed offset is added to
	the contents of GPR base to form the effective address.
*/
void m_lb(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lb $%s, %d($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	uint8_t value = m_memory_read((REGS[REGIDX_S] + SIMMDT), byte, m_simplestation);
	m_cpu_load_delay_enqueue_byte(REGIDX_T, value, m_simplestation);
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

	uint32_t value = m_memory_read((REGS[REGIDX_S] + SIMMDT), dword, m_simplestation);

	m_cpu_load_delay_enqueue_dword(REGIDX_T, value, m_simplestation);
}

/*
	LBU (MIPS I)
	Format:
	LBU rt, offset(base)
	Description:
	The contents of the 8-bit byte at the memory location specified by the effective address
	are fetched, zero-extended, and placed in GPR rt. The 16-bit signed offset is added to
	the contents of GPR base to form the effective address.
*/
void m_lbu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lbu $%s, %04X($%s)\n", m_cpu_regnames[REGIDX_T], (uint16_t) (SIMMDT & 0x0000FFFF), m_cpu_regnames[REGIDX_S]);
#endif

	uint8_t value = m_memory_read((REGS[REGIDX_S] + SIMMDT), byte, m_simplestation);

	m_cpu_load_delay_enqueue_byte(REGIDX_T, value, m_simplestation);
}

/*
	SB (MIPS I)

	Format:
	It is recommended that a reserved hint field value either cause a default
	prefetch action that is expected to be useful for most cases of data use, such
	as the “load” hint, or cause the instruction to be treated as a NOP. SB rt, offset(base)

	Description:
	To store a byte to memory.
	The least-significant 8-bit byte of GPR rt is stored in memory at the location specified
	by the effective address. The 16-bit signed offset is added to the contents of GPR base to
	form the effective address.
*/
void m_sb(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sb $%s, 0x%x($%s)\n", m_cpu_regnames[REGIDX_T], IMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	if ((COP0_SR & 0x10000) != 0)
	{
		printf(YELLOW "[CPU] sb: Ignoring word store, cache is isolated...\n" NORMAL);
		return;
	}
	
	m_memory_write(REGS[REGIDX_S] + SIMMDT, REGS[REGIDX_T], byte, m_simplestation);
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

	if ((COP0_SR & 0x10000) != 0)
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

	if ((COP0_SR & 0x10000) != 0)
	{
#ifdef DEBUG_INSTRUCTIONS
		printf(YELLOW "[CPU] sw: Ignoring word store, cache is isolated...\n" NORMAL);
#endif
		return;
	}

	m_memory_write((REGS[REGIDX_S] + SIMMDT), REGS[REGIDX_T], dword, m_simplestation);
}

/*
	ANDI (MIPS I)
	
	Format:
	ANDI rt, rs, immediate
	
	Description:
	To do a bitwise logical AND with a constant.
	The 16-bit immediate is zero-extended to the left and combined with the contents of
	GPR rs in a bitwise logical AND operation. The result is placed into GPR rt .
*/
void m_andi(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("andi $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], IMMDT);
#endif

	REGS[REGIDX_T] = (REGS[REGIDX_S] & IMMDT);
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