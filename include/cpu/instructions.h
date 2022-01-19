#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cpu/cpu.h>
#include <memory/memory.h>
#include <stdint.h>
#include <stdlib.h>

extern struct m_corewave_cw33300_instrs {
	const char *m_instr;
	void *m_funct;
} const m_psx_instrs[67];

/* Instructions */

// 0x00
void m_sll();

// 0x02
void m_j();

// 0x09
void m_addiu();

// 0x0B
void m_sw();

// 0x0D
void m_ori();

// 0x0F
void m_lui();

#endif /* INSTRUCTIONS_H */