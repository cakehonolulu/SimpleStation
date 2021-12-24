#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdlib.h>

extern struct m_corewave_cw33300_instrs {
	char *m_instr;
	uint32_t m_operand;
	void *m_funct;
} const m_mips_r3000a_instrs_t[67];

#endif /* INSTRUCTIONS_H */