#ifndef CPU_H
#define CPU_H

#include <memory/memory.h>
#include <cpu/instructions.h>
#include <debugger/debugger.h>
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

	/*
		High 32 bits of multiplication result
		Remainder of division
	*/
	uint32_t m_hi;

	/*
		Low 32 bits of multiplication result
		Quotient of division
	*/
	uint32_t m_lo;

	/*
		32 general prupose registers
	*/
	uint32_t m_registers[M_R3000_REGISTERS];

} m_mips_r3000a_t;

static const char *m_cpu_regnames[] = {
	"zr",
	"at",
	"v0", "v1",
	"a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9",
	"k0", "k1",
	"gp",
	"sp",
	"fp",
	"ra"
};

// Defined in cpu.c
extern m_mips_r3000a_t *m_cpu;
extern uint32_t m_opcode;

// Internal defines
#define REGS (m_cpu->m_registers)
#define INSTRUCTION ((uint32_t) (m_opcode >> 26))
#define IMMDT ((uint32_t) (m_opcode & 0xFFFF))
#define SIMMDT ((uint32_t) ((int16_t) (m_opcode & 0xFFFF)))
#define SUB ((uint32_t) (m_opcode & 0x3F))
#define SHIFT ((uint32_t) ((m_opcode >> 6) & 0x1F))
#define JIMMDT ((uint32_t) (m_opcode & 0x3FFFFFF))

// Operand Registers
#define REGIDX_S ((uint32_t) ((m_opcode >> 21) & 0x1F))
#define REGIDX_T ((uint32_t) ((m_opcode >> 16) & 0x1F))

// Recieving Register
#define REGIDX_D ((uint32_t) ((m_opcode >> 11) & 0x1F))

// Register defines
#define PC (m_cpu->m_pc)
#define HI (m_cpu->m_hi)
#define LO (m_cpu->m_lo)

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