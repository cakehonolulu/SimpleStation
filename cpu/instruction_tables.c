#include <cpu/instructions.h>

const struct m_corewave_cw33300_instrs m_psx_instrs[0x3F] = {
	{"exp", m_exp},		// 0x00
	{NULL, NULL},		// 0x01
	{"j", m_j},			// 0x02
	{"jal", m_jal},		// 0x03
	{NULL, NULL},		// 0x04
	{"bne", m_bne},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{"addi", m_addi},	// 0x08
	{"addiu", m_addiu},	// 0x09
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0B
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
	{NULL, NULL},		// 0x20
	{NULL, NULL},		// 0x01
	{NULL, NULL},		// 0x02
	{"lw", m_lw},		// 0x23
	{NULL, NULL},		// 0x04
	{NULL, NULL},		// 0x05
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
	{NULL, NULL},		// 0x02
	{NULL, NULL},		// 0x03
	{NULL, NULL},		// 0x04
	{NULL, NULL},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{NULL, NULL},		// 0x09
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0B
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0D
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x0F
	{NULL, NULL},		// 0x10
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
	{NULL, NULL},		// 0x20
	{"addu", m_addu},	// 0x21
	{NULL, NULL},		// 0x02
	{NULL, NULL},		// 0x03
	{NULL, NULL},		// 0x04
	{"or", m_or},		// 0x05
	{NULL, NULL},		// 0x06
	{NULL, NULL},		// 0x07
	{NULL, NULL},		// 0x08
	{NULL, NULL},		// 0x09
	{NULL, NULL},		// 0x2A
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

const struct m_corewave_cw33300_cop0_instrs m_psx_cop0[0x5] = {
	{NULL, NULL},		// 0x00
	{NULL, NULL},		// 0x01
	{NULL, NULL},		// 0x02
	{NULL, NULL},		// 0x03
	{"mtc0", m_mtc0}	// 0x04
};
