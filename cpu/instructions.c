#include <cpu/instructions.h>
#include <cpu/bios.h>
#include <ui/termcolour.h>
#include <debugger/debugger.h>

void m_exp(m_simplestation_state *m_simplestation)
{
	// Check if the instruction is implemented
	if (m_psx_extended_00[(m_simplestation->m_cpu->m_opcode & 0x3F)].m_funct == NULL)
	{
		printf(RED "[CPU] fde->exp: Illegal '0x00 Family' Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, (m_simplestation->m_cpu->m_opcode & 0x3F), (uint32_t) m_simplestation->m_cpu->m_opcode);	
		m_exc_types m_exc = illegal;
		m_exception(m_exc, m_simplestation);
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
		printf(RED "[CPU] fde->cop0: Illegal 'Coprocessor 0' Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, REGIDX_S, m_simplestation->m_cpu->m_opcode);
		m_exc_types m_exc = illegal;
		m_exception(m_exc, m_simplestation);
	}
	else
	{
		// Execute the instruction
		((void (*)(m_simplestation_state *m_simplestation))m_psx_cop0[REGIDX_S].m_funct)(m_simplestation);
	}
}

void m_cop1(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("cop2\n");
#endif

	m_exc_types m_exc = coprocessor;

	m_exception(m_exc, m_simplestation);
}

void m_cop2(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("cop2\n");
#endif

	printf(RED "[CPU] gte: Unimplemented 'GTE' Opcode: 0x%08X\n" NORMAL, m_simplestation->m_cpu->m_opcode);
	m_simplestation_exit(m_simplestation, 1);
}

void m_cop3(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("cop3\n");
#endif

	m_exc_types m_exc = coprocessor;

	m_exception(m_exc, m_simplestation);
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
	if (m_exception == illegal)
	{
		printf(RED "\n[CPU] fde: Illegal Opcode 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, INSTRUCTION, m_simplestation->m_cpu->m_opcode);
	}

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
	SLLV (MIPS I)

	Format:
	SLLV rd, rt, rs

	Description:
	The contents of the low-order 32-bit word of GPR rt are shifted left, inserting zeroes
	into the emptied bits; the result word is placed in GPR rd . The bit shift count is
	specified by the low-order five bits of GPR rs. If rd is a 64-bit register, the result word
	is sign-extended.
*/
void m_sllv(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sllv $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S]);
#endif

	REGS[REGIDX_D] = (REGS[REGIDX_T] << (REGS[REGIDX_S] & 0x1F));
}

/*
	SRLV (MIPS I)

	Format:
	SRLV rd, rt, rs

	Description:
	The contents of the low-order 32-bit word of GPR rt are shifted right, inserting zeros
	into the emptied bits; the word result is placed in GPR rd. The bit shift count is
	specified by the low-order five bits of GPR rs. If rd is a 64-bit register, the result word
	is sign-extended.
*/
void m_srlv(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("srlv $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S]);
#endif

	REGS[REGIDX_D] = ((uint32_t) (((int32_t) REGS[REGIDX_T]) >> (REGS[REGIDX_T] & 0x1F)));
}

/*
	SRAV (MIPS I)

	Format:
	SRAV rd, rt, rs

	Description:
	The contents of the low-order 32-bit word of GPR rt are shifted right, duplicating the
	sign-bit (bit 31) in the emptied bits; the word result is placed in GPR rd. The bit shift
	count is specified by the low-order five bits of GPR rs. If rd is a 64-bit register, the result
	word is sign-extended.
*/
void m_srav(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("srav $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S]);
#endif

	REGS[REGIDX_D] = ((REGS[REGIDX_T]) >> (REGS[REGIDX_T] & 0x1F));
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
#ifdef DEBUG_INSTRUCTIONS
	printf("syscall\n");
#endif

	m_exc_types m_exc = syscall;

	m_exception(m_exc, m_simplestation);
}

/*
	BREAK (MIPS I)

	Format:
	BREAK

	Description:
	A breakpoint exception occurs, immediately and unconditionally transferring control
	to the exception handler.
	The code field is available for use as software parameters, but is retrieved by the
	exception handler only by loading the contents of the memory word containing the
	instruction.
*/
void m_break(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("break\n");
#endif

	m_exc_types m_exc = breakpoint;

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
	MULT (MIPS I)

	Format:
	MULTU rs, rt

	Description:
	The 32-bit word value in GPR rt is multiplied by the 32-bit value in GPR rs, treating
	both operands as signed values, to produce a 64-bit result. The low-order 32-bit word
	of the result is placed into special register LO, and the high-order 32-bit word is placed
	into special register HI.
	No arithmetic exception occurs under any circumstances.
*/
void m_mult(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("mult $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	uint64_t m_res = ((((int64_t) ((int32_t) (REGS[REGIDX_S])))) * ((int64_t) (((int32_t) REGS[REGIDX_T]))));
	HI = ((uint32_t) (m_res >> 32));
	LO = ((uint32_t) (m_res));
}

/*
	MULTU (MIPS I)

	Format:
	MULTU rs, rt

	Description:
	The 32-bit word value in GPR rt is multiplied by the 32-bit value in GPR rs, treating
	both operands as unsigned values, to produce a 64-bit result. The low-order 32-bit
	word of the result is placed into special register LO, and the high-order 32-bit word is
	placed into special register HI.
	No arithmetic exception occurs under any circumstances.
*/
void m_multu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("multu $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	uint64_t m_res = ((((uint64_t) REGS[REGIDX_S])) * ((uint64_t) (REGS[REGIDX_T])));
	HI = ((uint32_t) (m_res >> 32));
	LO = ((uint32_t) (m_res));
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
		m_exc_types m_exc = overflow;
		m_exception(m_exc, m_simplestation);
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
	The 32-bit word value in GPR rt is subtracted from the 32-bit value in GPR rs to
	produce a 32-bit result. If the subtraction results in 32-bit 2’s complement arithmetic
	overflow then the destination register is not modified and an Integer Overflow
	exception occurs. If it does not overflow, the 32-bit result is placed into GPR rd.
*/
void m_sub(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("sub $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	if (m_cpu_check_signed_subtraction((int32_t) REGS[REGIDX_S], (int32_t) REGS[REGIDX_T]))
	{
		m_exc_types m_exc = overflow;
		m_exception(m_exc, m_simplestation);
	}
	else
	{
		REGS[REGIDX_D] = ((uint32_t) (((int32_t) REGS[REGIDX_S]) - ((int32_t) REGS[REGIDX_T])));
	}
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
	XOR (MIPS I)

	Format:
	XOR rd, rs, rt

	Description:
	Combine the contents of GPR rs and GPR rt in a bitwise logical exclusive OR operation
	and place the result into GPR rd.
*/
void m_xor(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("xor $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	REGS[REGIDX_D] = (REGS[REGIDX_S] ^ REGS[REGIDX_T]);
}

/*
	NOR (MIPS I)

	Format:
	NOR rd, rs, rt

	Description:
	The contents of GPR rs are combined with the contents of GPR rt in a bitwise logical
	NOR operation. The result is placed into GPR rd.
*/
void m_nor(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("nor $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
#endif

	REGS[REGIDX_D] = !(REGS[REGIDX_S] | REGS[REGIDX_T]);
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
		m_exc_types m_exc = overflow;
		m_exception(m_exc, m_simplestation);
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

	REGS[REGIDX_T] = ((int32_t) REGS[REGIDX_S]) < ((int32_t) SIMMDT);
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
	LH (MIPS I)

	Format:
	LH rt, offset(base)

	Description:
	The contents of the 16-bit halfword at the memory location specified by the aligned
	effective address are fetched, sign-extended, and placed in GPR rt. The 16-bit signed
	offset is added to the contents of GPR base to form the effective address.
*/
void m_lh(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lh $%s, %d($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	int16_t value = ((int16_t) (m_memory_read((REGS[REGIDX_S] + SIMMDT), word, m_simplestation)));

	// Make the value fit the entire register (We use _dword not _word)
	m_cpu_load_delay_enqueue_dword(REGIDX_T, ((uint32_t) value), m_simplestation);
}

/*
	LWL (MIPS I)

	Format:
	LWL rt, offset(base)

	Description:
	The 16-bit signed offset is added to the contents of GPR base to form an effective address
	(EffAddr). EffAddr is the address of the most-significant of four consecutive bytes
	forming a word in memory (W) starting at an arbitrary byte boundary. A part of W, the
	most-significant one to four bytes, is in the aligned word containing EffAddr. This part
	of W is loaded into the most-significant (left) part of the word in GPR rt. The remaining
	least-significant part of the word in GPR rt is unchanged.
	If GPR rt is a 64-bit register, the destination word is the low-order word of the register.
	The loaded value is treated as a signed value; the word sign bit (bit 31) is always loaded
	from memory and the new sign bit value is copied into bits 63..32.

	The figure above illustrates this operation for big-endian byte ordering for 32-bit and
	64-bit registers. The four consecutive bytes in 2..5 form an unaligned word starting at
	location 2. A part of W, two bytes, is in the aligned word containing the most-
	significant byte at 2. First, LWL loads these two bytes into the left part of the
	destination register word and leaves the right part of the destination word unchanged.
	Next, the complementary LWR loads the remainder of the unaligned word.
	The bytes loaded from memory to the destination register depend on both the offset of
	the effective address within an aligned word, i.e. the low two bits of the address
	(vAddr1..0), and the current byte ordering mode of the processor (big- or little-endian).
	The table below shows the bytes loaded for every combination of offset and byte
	ordering.

	The unaligned loads, LWL and LWR, are exceptions to the load-delay scheduling
	restriction in the MIPS I architecture. An unaligned load instruction to GPR rt that
	immediately follows another load to GPR rt can “read” the loaded data. It will
	correctly merge the 1 to 4 loaded bytes with the data loaded by the previous
	instruction.
*/
void m_lwl(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lwl $%s, %d($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	uint32_t m_addr = REGS[REGIDX_S] + SIMMDT;

	uint32_t m_aligned = m_addr & !3;

	uint32_t m_dword = m_memory_read(m_aligned, dword, m_simplestation);

	uint32_t m_val;

	switch (m_addr & 3)
	{
		case 0:
			m_val = ((REGS[REGIDX_T] & 0x00FFFFFF) | (m_dword << 24));
			break;
		
		case 1:
			m_val = ((REGS[REGIDX_T] & 0x0000FFFF) | (m_dword << 16));
			break;
		
		case 2:
			m_val = ((REGS[REGIDX_T] & 0x000000FF) | (m_dword << 8));
			break;
		
		case 3:
			m_val = ((REGS[REGIDX_T] & 0x00000000) | (m_dword << 0));
			break;

		default:
			__builtin_unreachable();
	}

	m_cpu_load_delay_enqueue_dword(REGIDX_T, m_val, m_simplestation);
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
	LHU (MIPS I)
	
	Format:
	LHU rt, offset(base)

	Description:
	The contents of the 16-bit halfword at the memory location specified by the aligned
	effective address are fetched, zero-extended, and placed in GPR rt . The 16-bit signed
	offset is added to the contents of GPR base to form the effective address.
*/
void m_lhu(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lhu $%s, %04X($%s)\n", m_cpu_regnames[REGIDX_T], (uint16_t) (SIMMDT & 0x0000FFFF), m_cpu_regnames[REGIDX_S]);
#endif

	uint16_t value = m_memory_read((REGS[REGIDX_S] + SIMMDT), word, m_simplestation);

	m_cpu_load_delay_enqueue_word(REGIDX_T, value, m_simplestation);
}

/*
	LWR (MIPS I)

	Format:
	LWR rt, offset(base)

	Description:
	The figure above illustrates this operation for big-endian byte ordering for 32-bit and
	64-bit registers. The four consecutive bytes in 2..5 form an unaligned word starting at
	location 2. A part of W, two bytes, is in the aligned word containing the least-significant
	byte at 5. First, LWR loads these two bytes into the right part of the destination register.
	Next, the complementary LWL loads the remainder of the unaligned word.
	The bytes loaded from memory to the destination register depend on both the offset of
	the effective address within an aligned word, i.e. the low two bits of the address
	(vAddr1..0), and the current byte ordering mode of the processor (big- or little-endian).
	The table below shows the bytes loaded for every combination of offset and byte
	ordering.

	The unaligned loads, LWL and LWR, are exceptions to the load-delay scheduling
	restriction in the MIPS I architecture. An unaligned load to GPR rt that immediately
	follows another load to GPR rt can “read” the loaded data. It will correctly merge the
	1 to 4 loaded bytes with the data loaded by the previous instruction.
*/
void m_lwr(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lwr $%s, %d($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	uint32_t m_addr = REGS[REGIDX_S] + SIMMDT;

	uint32_t m_aligned = m_addr & !3;

	uint32_t m_dword = m_memory_read(m_aligned, dword, m_simplestation);

	uint32_t m_val;

	switch (m_addr & 3)
	{
		case 0:
			m_val = ((REGS[REGIDX_T] & 0x00000000) | (m_dword << 0));
			break;
		
		case 1:
			m_val = ((REGS[REGIDX_T] & 0xFF000000) | (m_dword << 8));
			break;
		
		case 2:
			m_val = ((REGS[REGIDX_T] & 0xFFFF0000) | (m_dword << 16));
			break;
		
		case 3:
			m_val = ((REGS[REGIDX_T] & 0xFFFFFF00) | (m_dword << 24));
			break;

		default:
			__builtin_unreachable();
	}
	
	m_cpu_load_delay_enqueue_dword(REGIDX_T, m_val, m_simplestation);
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
	SWL (MIPS I)

	Format:
	SWL rt, offset(base)

	Description:
	The 16-bit signed offset is added to the contents of GPR base to form an effective address
	(EffAddr). EffAddr is the address of the most-significant of four consecutive bytes
	forming a word in memory (W) starting at an arbitrary byte boundary. A part of W, the
	most-significant one to four bytes, is in the aligned word containing EffAddr. The same
	number of the most-significant (left) bytes from the word in GPR rt are stored into
	these bytes of W.
	If GPR rt is a 64-bit register, the source word is the low word of the register.
	Figures A-2 illustrates this operation for big-endian byte ordering for 32-bit and 64-bit
	registers. The four consecutive bytes in 2..5 form an unaligned word starting at
	location 2. A part of W, two bytes, is contained in the aligned word containing the
	most-significant byte at 2. First, SWL stores the most-significant two bytes of the low-
	word from the source register into these two bytes in memory. Next, the
	complementary SWR stores the remainder of the unaligned word.

	The bytes stored from the source register to memory depend on both the offset of the
	effective address within an aligned word, i.e. the low two bits of the address
	(vAddr1..0), and the current byte ordering mode of the processor (big- or little-endian).
	The table below shows the bytes stored for every combination of offset and byte
	ordering
*/
void m_swl(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("swl $%s, 0x%x($%s)\n", m_cpu_regnames[REGIDX_T], IMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	uint32_t m_aligned_addr = ((REGS[REGIDX_S] + SIMMDT) & !3);

	uint32_t m_val = m_memory_read(m_aligned_addr, dword, m_simplestation);

	switch ((REGS[REGIDX_S] + SIMMDT) & 3)
	{
		case 0:
			m_memory_write(m_aligned_addr, ((m_val & 0xFFFFFF00) | (REGS[REGIDX_T] >> 24)), dword, m_simplestation);
			break;

		case 1:
			m_memory_write(m_aligned_addr, ((m_val & 0xFFFF0000) | (REGS[REGIDX_T] >> 16)), dword, m_simplestation);
			break;

		case 2:
			m_memory_write(m_aligned_addr, ((m_val & 0xFF000000) | (REGS[REGIDX_T] >> 8)), dword, m_simplestation);
			break;

		case 3:
			m_memory_write(m_aligned_addr, ((m_val & 0x00000000) | (REGS[REGIDX_T] >> 0)), dword, m_simplestation);
			break;

		default:
			__builtin_unreachable();
	}
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
	SWR (MIPS I)

	Format:
	SWR rt, offset(base)
	
	Description:
	The 16-bit signed offset is added to the contents of GPR base to form an effective address
	(EffAddr). EffAddr is the address of the least-significant of four consecutive bytes
	forming a word in memory (W) starting at an arbitrary byte boundary. A part of W, the
	least-significant one to four bytes, is in the aligned word containing EffAddr. The same
	number of the least-significant (right) bytes from the word in GPR rt are stored into
	these bytes of W.
	If GPR rt is a 64-bit register, the source word is the low word of the register.
	Figures A-2 illustrates this operation for big-endian byte ordering for 32-bit and 64-bit
	registers. The four consecutive bytes in 2..5 form an unaligned word starting at
	location 2. A part of W, two bytes, is contained in the aligned word containing the least-
	significant byte at 5. First, SWR stores the least-significant two bytes of the low-word
	from the source register into these two bytes in memory. Next, the complementary
	SWL stores the remainder of the unaligned word.

	The bytes stored from the source register to memory depend on both the offset of the
	effective address within an aligned word, i.e. the low two bits of the address
	(vAddr1..0), and the current byte ordering mode of the processor (big- or little-endian).
	The tabel below shows the bytes stored for every combination of offset and byte
	ordering
*/
void m_swr(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("swr $%s, 0x%X($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
#endif

	uint32_t m_addr = REGS[REGIDX_S] + SIMMDT;

	uint32_t m_val = m_memory_read((m_addr & !3), dword, m_simplestation);

	switch (m_addr & 3)
	{
		case 0:
			m_memory_write((m_addr & !3), ((m_val & 0x00000000) | (REGS[REGIDX_T] << 0)), dword, m_simplestation);
			break;

		case 1:
			m_memory_write((m_addr & !3), ((m_val & 0xFF000000) | (REGS[REGIDX_T] << 8)), dword, m_simplestation);
			break;

		case 2:
			m_memory_write((m_addr & !3), ((m_val & 0xFFFF0000) | (REGS[REGIDX_T] << 16)), dword, m_simplestation);
			break;

		case 3:
			m_memory_write((m_addr & !3), ((m_val & 0xFFFFFF00) | (REGS[REGIDX_T] << 24)), dword, m_simplestation);
			break;

		default:
			__builtin_unreachable();
	}
}

void m_lwc0(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lwc0\n");
#endif

	m_exc_types m_exc = coprocessor;
	m_exception(m_exc, m_simplestation);

}

void m_lwc1(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lwc1\n");
#endif

	m_exc_types m_exc = coprocessor;
	m_exception(m_exc, m_simplestation);
}

void m_lwc2(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("lwc2\n");
#endif

	printf(RED "[CPU] lwc2: Unhandled instr.\n" NORMAL);
	m_simplestation_exit(m_simplestation, 1);
}

void m_lwc3(m_simplestation_state *m_simplestation)
{

#ifdef DEBUG_INSTRUCTIONS
	printf("lwc3\n");
#endif

	m_exc_types m_exc = coprocessor;
	m_exception(m_exc, m_simplestation);
}

void m_swc0(m_simplestation_state *m_simplestation)
{

#ifdef DEBUG_INSTRUCTIONS
	printf("swc0\n");
#endif

	m_exc_types m_exc = coprocessor;
	m_exception(m_exc, m_simplestation);
}

void m_swc1(m_simplestation_state *m_simplestation)
{

#ifdef DEBUG_INSTRUCTIONS
	printf("swc1\n");
#endif

	m_exc_types m_exc = coprocessor;
	m_exception(m_exc, m_simplestation);
}

void m_swc2(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("swc2\n");
#endif

	printf(RED "[CPU] swc2: Unhandled instr.\n" NORMAL);
	m_simplestation_exit(m_simplestation, 1);
}

void m_swc3(m_simplestation_state *m_simplestation)
{

#ifdef DEBUG_INSTRUCTIONS
	printf("swc3\n");
#endif

	m_exc_types m_exc = coprocessor;
	m_exception(m_exc, m_simplestation);
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
	XORI (MIPS I)

	Format:
	XORI rt, rs, immediate
	
	Description:
	Combine the contents of GPR rs and the 16-bit zero-extended immediate in a bitwise
	logical exclusive OR operation and place the result into GPR rt.
*/
void m_xori(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_INSTRUCTIONS
	printf("xori $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], IMMDT);
#endif

	REGS[REGIDX_T] = REGS[REGIDX_S] ^ IMMDT;
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
