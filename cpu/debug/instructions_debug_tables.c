#include <cpu/instructions_debug.h>

const struct m_opc_names m_psx_instrs_opcodes[0x3F] = {
	{"exp", p_exp},		// 0x00
	{"bxx", p_bxx},		// 0x01
	{"j", p_j},			// 0x02
	{"jal", p_jal},		// 0x03
	{"beq", p_beq},		// 0x04
	{"bne", p_bne},		// 0x05
	{"blez", p_blez},	// 0x06
	{"bgtz", p_bgtz},	// 0x07
	{"addi", p_addi},	// 0x08
	{"addiu", p_addiu},	// 0x09
	{"slti", p_slti},	// 0x00
	{"sltiu", p_sltiu},	// 0x0B
	{"andi", p_andi},	// 0x0C
	{"ori", p_ori},		// 0x0D
	{NULL, NULL},		// 0x00
	{"lui", p_lui},		// 0x0F
	{"cop0", p_cop0},	// 0x10
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
	{"lb", p_lb},		// 0x20
	{NULL, NULL},		// 0x01
	{NULL, NULL},		// 0x02
	{"lw", p_lw},		// 0x23
	{"lbu", p_lbu},		// 0x24
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{"sb", p_sb},		// 0x28
	{"sh", p_sh},		// 0x29
	{NULL, NULL},		// 0x2A
	{"sw", p_sw},		// 0x2B
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

const struct m_opc_ext_names m_psx_extended_00_opcodes[0x3F] = {
	{"sll", p_sll},		// 0x00
	{NULL, NULL},		// 0x01
	{"srl", p_srl},		// 0x02
	{"sra", p_sra},		// 0x03
	{NULL, NULL},		// 0x04
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{"jr", p_jr},		// 0x08
	{"jalr", p_jalr},	// 0x09
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0B
	{"syscall", p_syscall},	// 0x00
	{NULL, NULL},		// 0x0D
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0F
	{"mfhi", p_mfhi},	// 0x10
	{NULL, NULL},		// 0x11
	{"mflo", p_mflo},	// 0x12
	{NULL, NULL},		// 0x03
	{NULL, NULL},		// 0x04
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{NULL, NULL},		// 0x09
	{"div", p_div},		// 0x1A
	{"divu", p_divu},	// 0x1B
	{NULL, NULL},		// 0x0C
	{NULL, NULL},		// 0x0D
	{NULL, NULL},		// 0x0E
	{NULL, NULL},		// 0x0F
	{"add", p_add},		// 0x20
	{"addu", p_addu},	// 0x21
	{NULL, NULL},		// 0x02
	{"subu", p_subu},	// 0x23
	{"and", p_and},		// 0x24
	{"or", p_or},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{NULL, NULL},		// 0x09
	{"slt", p_slt},		// 0x2A
	{"sltu", p_sltu},	// 0x2B
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

const struct m_opc_cop0_names m_psx_cop0_opcodes[0x5] = {
	{"mfc0", p_mfc0},	// 0x00
	{NULL, NULL},		// 0x01
	{NULL, NULL},		// 0x02
	{NULL, NULL},		// 0x03
	{"mtc0", p_mtc0}	// 0x04
};