
#include <cpu/instructions_debug.h>

void p_exp(m_simplestation_state *m_simplestation)
{
	// Check if the instruction is implemented
	if (m_psx_extended_00_opcodes[(m_simplestation->m_cpu->m_opcode & 0x3F)].m_funct == NULL)
	{
		printf(RED "[CPU] fde->exp: Unimplemented '0x00 Family' Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, (m_simplestation->m_cpu->m_opcode & 0x3F), (uint32_t) m_simplestation->m_cpu->m_opcode);
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		// Execute the instruction
		((void (*)(m_simplestation_state *m_simplestation))m_psx_extended_00_opcodes[(m_simplestation->m_cpu->m_opcode & 0x3F)].m_funct)(m_simplestation);
	}
}

void p_cop0(m_simplestation_state *m_simplestation)
{
	// Check if the instruction is implemented
	if (m_psx_cop0_opcodes[REGIDX_S].m_funct == NULL)
	{
		printf(RED "[CPU] fde->cop0: Unimplemented 'Coprocessor 0' Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, REGIDX_S, m_simplestation->m_cpu->m_opcode);
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		// Execute the instruction
		((void (*)(m_simplestation_state *m_simplestation))m_psx_cop0_opcodes[REGIDX_S].m_funct)(m_simplestation);
	}
}

void p_cop1(m_simplestation_state *m_simplestation)
{
	(void) m_simplestation;
	printf("cop1\n");
}

void p_cop2(m_simplestation_state *m_simplestation)
{
	(void) m_simplestation;
	printf("cop2\n");
}

void p_cop3(m_simplestation_state *m_simplestation)
{
	(void) m_simplestation;
	printf("cop3\n");
}

void p_mfc0(m_simplestation_state *m_simplestation)
{
	printf("mfc0 $%s, $%s\n", m_cpu_regnames[REGIDX_T], m_cop0_regnames[REGIDX_D]);
}

void p_mtc0(m_simplestation_state *m_simplestation)
{
	printf("mtc0 $%s, $%s\n", m_cpu_regnames[REGIDX_T], m_cop0_regnames[REGIDX_D]);
}

void p_rfe(m_simplestation_state *m_simplestation)
{
	(void) (m_simplestation);
	printf("rfe\n");
}

void p_bxx(m_simplestation_state *m_simplestation)
{
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
}

void p_sll(m_simplestation_state *m_simplestation)
{
	if (m_simplestation->m_cpu->m_opcode == 0)
	{
		printf("nop\n");
	}
	else
	{
		printf("sll $%s, $%s, %x\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
	}
}

void p_srl(m_simplestation_state *m_simplestation)
{
	printf("srl $%s, $%s, %x\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
}

void p_sra(m_simplestation_state *m_simplestation)
{
	printf("sra $%s, $%s, %d\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], SHIFT);
}

void p_sllv(m_simplestation_state *m_simplestation)
{
	printf("sllv $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S]);
}

void p_srlv(m_simplestation_state *m_simplestation)
{
	printf("srlv $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S]);
}

void p_srav(m_simplestation_state *m_simplestation)
{
	printf("srav $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S]);
}

void p_jr(m_simplestation_state *m_simplestation)
{
	printf("jr $%s\n", m_cpu_regnames[REGIDX_S]);
}

void p_jalr(m_simplestation_state *m_simplestation)
{
	if (REGIDX_D == 31)
	{
		printf("jalr $%s\n", m_cpu_regnames[REGIDX_S]);
	}
	else
	{
		printf("jalr $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S]);
	}
}

void p_syscall(m_simplestation_state *m_simplestation)
{
    (void) (m_simplestation);
    printf("syscall\n");
}

void p_break(m_simplestation_state *m_simplestation)
{
    (void) (m_simplestation);
    printf("break\n");
}

void p_mfhi(m_simplestation_state *m_simplestation)
{
	printf("mfhi $%s\n", m_cpu_regnames[REGIDX_D]);
}

void p_mthi(m_simplestation_state *m_simplestation)
{
	printf("mthi $%s\n", m_cpu_regnames[REGIDX_S]);
}

void p_mflo(m_simplestation_state *m_simplestation)
{
	printf("mflo $%s\n", m_cpu_regnames[REGIDX_D]);
}

void p_mtlo(m_simplestation_state *m_simplestation)
{
	printf("mtlo $%s\n", m_cpu_regnames[REGIDX_S]);
}

void p_mult(m_simplestation_state *m_simplestation)
{
	printf("mult $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_multu(m_simplestation_state *m_simplestation)
{
	printf("multu $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_div(m_simplestation_state *m_simplestation)
{
	printf("div $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_divu(m_simplestation_state *m_simplestation)
{
	printf("divu $%s, $%s\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_add(m_simplestation_state *m_simplestation)
{
	printf("add $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_addu(m_simplestation_state *m_simplestation)
{
	printf("addu $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_sub(m_simplestation_state *m_simplestation)
{
	printf("sub $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_subu(m_simplestation_state *m_simplestation)
{
	printf("subu $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_and(m_simplestation_state *m_simplestation)
{
	printf("and $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_or(m_simplestation_state *m_simplestation)
{
	printf("or $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_xor(m_simplestation_state *m_simplestation)
{
	printf("xor $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_nor(m_simplestation_state *m_simplestation)
{
	printf("nor $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_slt(m_simplestation_state *m_simplestation)
{
	printf("slt $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_sltu(m_simplestation_state *m_simplestation)
{
	printf("sltu $%s, $%s, $%s\n", m_cpu_regnames[REGIDX_D], m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T]);
}

void p_j(m_simplestation_state *m_simplestation)
{
	printf("j 0x%x\n", ((PC & 0xF0000000) | (JIMMDT * 4)));
}

void p_jal(m_simplestation_state *m_simplestation)
{
	printf("jal 0x%x\n", ((PC & 0xF0000000) | (JIMMDT * 4)));
}

void p_beq(m_simplestation_state *m_simplestation)
{
	printf("beq $%s, $%s, %d\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T], (SIMMDT) << 2);
}

void p_bne(m_simplestation_state *m_simplestation)
{
	printf("bne $%s, $%s, %d\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T], (SIMMDT) << 2);
}

void p_blez(m_simplestation_state *m_simplestation)
{
	printf("blez $%s, %d\n", m_cpu_regnames[REGIDX_S], (SIMMDT) << 2);
}

void p_bgtz(m_simplestation_state *m_simplestation)
{
	printf("bgtz $%s, %d\n", m_cpu_regnames[REGIDX_S], (SIMMDT) << 2);
}

void p_addi(m_simplestation_state *m_simplestation)
{
	printf("addi $%s, $%s, %d\n", m_cpu_regnames[REGIDX_S], m_cpu_regnames[REGIDX_T], SIMMDT);
}

void p_addiu(m_simplestation_state *m_simplestation)
{
	printf("addiu $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], SIMMDT);
}

void p_slti(m_simplestation_state *m_simplestation)
{
	printf("slti $%s, $%s, %d\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], SIMMDT);
}

void p_sltiu(m_simplestation_state *m_simplestation)
{
	printf("sltiu $%s, $%s, %d\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], SIMMDT);
}

void p_lb(m_simplestation_state *m_simplestation)
{
	printf("lb $%s, %d($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
}

void p_lh(m_simplestation_state *m_simplestation)
{
	printf("lh $%s, %d($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
}

void p_lw(m_simplestation_state *m_simplestation)
{
	printf("lw $%s, 0x%x($%x)\n", m_cpu_regnames[REGIDX_T], IMMDT, REGIDX_S);
}

void p_lbu(m_simplestation_state *m_simplestation)
{
	printf("lbu $%s, %04X($%s)\n", m_cpu_regnames[REGIDX_T], (uint16_t) (SIMMDT & 0x0000FFFF), m_cpu_regnames[REGIDX_S]);
}

void p_lhu(m_simplestation_state *m_simplestation)
{
	printf("lhu $%s, %04X($%s)\n", m_cpu_regnames[REGIDX_T], (uint16_t) (SIMMDT & 0x0000FFFF), m_cpu_regnames[REGIDX_S]);
}

void p_sb(m_simplestation_state *m_simplestation)
{
	printf("sb $%s, 0x%x($%s)\n", m_cpu_regnames[REGIDX_T], IMMDT, m_cpu_regnames[REGIDX_S]);
}

void p_sh(m_simplestation_state *m_simplestation)
{
	printf("sh $%s, 0x%x($%s)\n", m_cpu_regnames[REGIDX_T], IMMDT, m_cpu_regnames[REGIDX_S]);
}

void p_swl(m_simplestation_state *m_simplestation)
{
	printf("swl $%s, 0x%X($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
}

void p_sw(m_simplestation_state *m_simplestation)
{
	printf("sw $%s, 0x%X($%s)\n", m_cpu_regnames[REGIDX_T], SIMMDT, m_cpu_regnames[REGIDX_S]);
}

void p_andi(m_simplestation_state *m_simplestation)
{
	printf("andi $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], IMMDT);
}

void p_ori(m_simplestation_state *m_simplestation)
{
	printf("ori $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], IMMDT);
}

void p_xori(m_simplestation_state *m_simplestation)
{
	printf("xori $%s, $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], m_cpu_regnames[REGIDX_S], IMMDT);
}

void p_lui(m_simplestation_state *m_simplestation)
{
	printf("lui $%s, 0x%X\n", m_cpu_regnames[REGIDX_T], IMMDT);
}
