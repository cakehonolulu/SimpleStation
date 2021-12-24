#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>

extern struct m_corewave_cw33300_opcodes {
	char *m_instr;
	uint32_t m_operand;
	void *m_funct;
} const m_mips_r3000a_opcodes_t[67];

#endif /* CPU_H */