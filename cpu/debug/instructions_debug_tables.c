#include <cpu/instructions_debug.h>

const struct m_opc_names m_psx_instrs_opcodes[0x3F] = {
	{p_exp}, 	{p_bxx},	{p_j},		{p_jal},	{p_beq},	{p_bne},	{p_blez},	{p_bgtz},
	{p_addi},	{p_addiu},	{p_slti},	{p_sltiu},	{p_andi},	{p_ori},	{p_xori},	{p_lui},
	{p_cop0},	{p_cop1},	{p_cop2},	{p_cop3},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{p_lb},		{p_lh},		{ILLEGAL},	{p_lw},		{p_lbu},	{p_lhu},	{ILLEGAL},	{ILLEGAL},
	{p_sb},		{p_sh},		{p_swl},	{p_sw},		{ILLEGAL},	{ILLEGAL},	{p_swr},	{ILLEGAL},
	{p_lwc0},	{p_lwc1},	{p_lwc2},	{p_lwc3},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{p_swc0},	{p_swc1},	{p_swc2},	{p_swc3},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL}
};

const struct m_opc_ext_names m_psx_extended_00_opcodes[0x3F] = {
	{p_sll},	{ILLEGAL},	{p_srl},	{p_sra},	{p_sllv},	{ILLEGAL},	{p_srlv},	{p_srav},
	{p_jr},		{p_jalr},	{ILLEGAL},	{ILLEGAL},	{p_syscall},{p_break},	{ILLEGAL},	{ILLEGAL},
	{p_mfhi},	{p_mthi},	{p_mflo},	{p_mtlo},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{p_mult},	{p_multu},	{p_div},	{p_divu},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{p_add},	{p_addu},	{p_sub},	{p_subu},	{p_and},	{p_or},		{p_xor},	{p_nor},
	{ILLEGAL},	{ILLEGAL},	{p_slt},	{p_sltu},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL}
};

const struct m_opc_cop0_names m_psx_cop0_opcodes[0x11] = {
	{p_mfc0},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{p_mtc0},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{p_rfe}		
};
