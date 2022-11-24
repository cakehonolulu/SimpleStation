#include <cpu/instructions.h>

const struct m_corewave_cw33300_instrs m_psx_instrs[0x3F] = {
	{"exp", m_exp},		// 0x00
	{"bxx", m_bxx},		// 0x01
	{"j", m_j},			// 0x02
	{"jal", m_jal},		// 0x03
	{"beq", m_beq},		// 0x04
	{"bne", m_bne},		// 0x05
	{"blez", m_blez},	// 0x06
	{"bgtz", m_bgtz},	// 0x07
	{"addi", m_addi},	// 0x08
	{"addiu", m_addiu},	// 0x09
	{"slti", m_slti},	// 0x00
	{"sltiu", m_sltiu},	// 0x0B
	{"andi", m_andi},	// 0x0C
	{"ori", m_ori},		// 0x0D
	{NULL, NULL},		// 0x00
	{"lui", m_lui},		// 0x0F
	{"cop0", m_cop0},	// 0x10
	{NULL, NULL},		// 0x11
	{NULL, NULL},		// 0x02
	{NULL, NULL},		// 0x03
	{NULL, NULL},		// 0x04
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{NULL, NULL},		// 0x09
	{NULL, NULL},		// 0x0A
	{NULL, NULL},		// 0x0B
	{NULL, NULL},		// 0x0C
	{NULL, NULL},		// 0x0D
	{NULL, NULL},		// 0x0E
	{NULL, NULL},		// 0x0F
	{"lb", m_lb},		// 0x20
	{"lh", m_lh},		// 0x21
	{NULL, NULL},		// 0x02
	{"lw", m_lw},		// 0x23
	{"lbu", m_lbu},		// 0x24
	{"lhu", m_lhu},		// 0x25
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{"sb", m_sb},		// 0x28
	{"sh", m_sh},		// 0x29
	{NULL, NULL},		// 0x2A
	{"sw", m_sw},		// 0x2B
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

const struct m_corewave_cw33300_extended_00_instrs m_psx_extended_00[0x3F] = {
	{"sll", m_sll},		// 0x00
	{NULL, NULL},		// 0x01
	{"srl", m_srl},		// 0x02
	{"sra", m_sra},		// 0x03
	{"sllv", m_sllv},	// 0x04
	{NULL, NULL},		// 0x05
	{"srlv", m_srlv},	// 0x06
	{"srav", m_srav},	// 0x07
	{"jr", m_jr},		// 0x08
	{"jalr", m_jalr},	// 0x09
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0B
	{"syscall", m_syscall},	// 0x00
	{NULL, NULL},		// 0x0D
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0F
	{"mfhi", m_mfhi},	// 0x10
	{"mthi", m_mthi},	// 0x11
	{"mflo", m_mflo},	// 0x12
	{"mtlo", m_mtlo},	// 0x13
	{NULL, NULL},		// 0x04
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{"multu", m_multu},	// 0x19
	{"div", m_div},		// 0x1A
	{"divu", m_divu},	// 0x1B
	{NULL, NULL},		// 0x0C
	{NULL, NULL},		// 0x0D
	{NULL, NULL},		// 0x0E
	{NULL, NULL},		// 0x0F
	{"add", m_add},		// 0x20
	{"addu", m_addu},	// 0x21
	{NULL, NULL},		// 0x02
	{"subu", m_subu},	// 0x23
	{"and", m_and},		// 0x24
	{"or", m_or},		// 0x05
	{NULL, NULL},		// 0x06
	{"nor", m_nor},		// 0x27
	{NULL, NULL},		// 0x08
	{NULL, NULL},		// 0x09
	{"slt", m_slt},		// 0x2A
	{"sltu", m_sltu},	// 0x2B
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

const struct m_corewave_cw33300_cop0_instrs m_psx_cop0[0x11] = {
	{"mfc0", m_mfc0},	// 0x00
	{NULL, NULL},		// 0x01
	{NULL, NULL},		// 0x02
	{NULL, NULL},		// 0x03
	{"mtc0", m_mtc0},	// 0x04
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{NULL, NULL},		// 0x09
	{NULL, NULL},		// 0x0A
	{NULL, NULL},		// 0x0B
	{NULL, NULL},		// 0x0C
	{NULL, NULL},		// 0x0D
	{NULL, NULL},		// 0x0E
	{NULL, NULL},		// 0x0F
	{"rfe", m_rfe}		// 0x10
};
