#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*
	Sony Playstation 1 (PSX) CPU Header
	
	LSI CoreWare CW33300-based core:
	MIPS R3000A-compatible 32-bit RISC CPU (MIPS R3051)
	5 KB L1 cache @ 33.8688MHz
*/

// Struct containing main CPU state
typedef struct m_corewave_cw33300
{
	// Program Counter Register
	uint32_t m_pc;

} m_mips_r3000a_t;

// Defined in cpu.c
extern m_mips_r3000a_t *m_cpu;

#define PC (m_cpu->m_pc)

/* Function definitions */
void m_cpu_init();
void m_cpu_exit();

#endif /* CPU_H */