#include <cpu/instructions.h>

const struct m_corewave_cw33300_instrs m_psx_instrs[0x3F] = {
	{m_exp}, 	{m_bxx},	{m_j},		{m_jal},	{m_beq},	{m_bne},	{m_blez},	{m_bgtz},
	{m_addi},	{m_addiu},	{m_slti},	{m_sltiu},	{m_andi},	{m_ori},	{m_xori},	{m_lui},
	{m_cop0},	{m_cop1},	{m_cop2},	{m_cop3},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{m_lb},		{m_lh},		{ILLEGAL},	{m_lw},		{m_lbu},	{m_lhu},	{ILLEGAL},	{ILLEGAL},
	{m_sb},		{m_sh},		{m_swl},	{m_sw},		{ILLEGAL},	{ILLEGAL},	{m_swr},	{ILLEGAL},
	{m_lwc0},	{m_lwc1},	{m_lwc2},	{m_lwc3},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{m_swc0},	{m_swc1},	{m_swc2},	{m_swc3},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL}
};

const struct m_corewave_cw33300_extended_00_instrs m_psx_extended_00[0x3F] = {
	{m_sll},	{ILLEGAL},	{m_srl},	{m_sra},	{m_sllv},	{ILLEGAL},	{m_srlv},	{m_srav},
	{m_jr},		{m_jalr},	{ILLEGAL},	{ILLEGAL},	{m_syscall},{m_break},	{ILLEGAL},	{ILLEGAL},
	{m_mfhi},	{m_mthi},	{m_mflo},	{m_mtlo},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{m_mult},	{m_multu},	{m_div},	{m_divu},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{m_add},	{m_addu},	{m_sub},	{m_subu},	{m_and},	{m_or},		{m_xor},	{m_nor},
	{ILLEGAL},	{ILLEGAL},	{m_slt},	{m_sltu},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL}
};

const struct m_corewave_cw33300_cop0_instrs m_psx_cop0[0x11] = {
	{m_mfc0},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{m_mtc0},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},	{ILLEGAL},
	{m_rfe}	
};
