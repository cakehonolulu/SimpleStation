#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdlib.h>

extern struct m_corewave_cw33300_instrs {
	uint8_t *m_instr;
	void *m_funct;
} const m_mips_r3000a_instrs_t[67];

#endif /* INSTRUCTIONS_H */