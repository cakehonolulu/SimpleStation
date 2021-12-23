#ifndef CPU_H
#define CPU_H

#include <memory/memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define M_R3000_REGISTERS 32

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

	uint32_t m_registers[M_R3000_REGISTERS];

} m_mips_r3000a_t;

// Defined in cpu.c
extern m_mips_r3000a_t *m_cpu;

// Register defines
#define PC (m_cpu->m_pc)
#define ZERO (m_cpu->m_registers[0])
#define AT (m_cpu->m_registers[1])
#define V0 (m_cpu->m_registers[2])
#define V1 (m_cpu->m_registers[3])
#define A0 (m_cpu->m_registers[4])
#define A1 (m_cpu->m_registers[5])
#define A2 (m_cpu->m_registers[6])
#define A3 (m_cpu->m_registers[7])
#define T0 (m_cpu->m_registers[8])
#define T1 (m_cpu->m_registers[9])
#define T2 (m_cpu->m_registers[10])
#define T3 (m_cpu->m_registers[11])
#define T4 (m_cpu->m_registers[12])
#define T5 (m_cpu->m_registers[13])
#define T6 (m_cpu->m_registers[14])
#define T7 (m_cpu->m_registers[15])
#define S0 (m_cpu->m_registers[16])
#define S1 (m_cpu->m_registers[17])
#define S2 (m_cpu->m_registers[18])
#define S3 (m_cpu->m_registers[19])
#define S4 (m_cpu->m_registers[20])
#define S5 (m_cpu->m_registers[21])
#define S6 (m_cpu->m_registers[22])
#define S7 (m_cpu->m_registers[23])
#define T8 (m_cpu->m_registers[24])
#define T9 (m_cpu->m_registers[25])
#define K0 (m_cpu->m_registers[26])
#define K1 (m_cpu->m_registers[27])
#define GP (m_cpu->m_registers[28])
#define SP (m_cpu->m_registers[29])
#define FP (m_cpu->m_registers[30])
#define RA (m_cpu->m_registers[31])

/* Function definitions */
void m_cpu_init();
void m_cpu_exit();
void m_cpu_fetch();
void m_cpu_decode();
void m_cpu_execute();

#endif /* CPU_H */