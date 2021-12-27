#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cpu/cpu.h>
#include <stdint.h>
#include <stdlib.h>

extern struct m_corewave_cw33300_instrs {
	const char *m_instr;
	void *m_funct;
} const m_psx_instrs[67];

/* Instructions */

// 0x0F
void m_lui();

#endif /* INSTRUCTIONS_H */