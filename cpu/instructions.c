#include <cpu/instructions.h>

const struct m_corewave_cw33300_instrs m_psx_instrs[67] = {
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{"lui", m_lui},	// 0x0F
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL},	// 0x00
	{NULL, NULL}	// 0x00
};

void m_lui()
{
	uint32_t v = m_immediate << 16;

	m_cpu->m_registers[m_regidx - 1] = v;

	PC += 4;
}