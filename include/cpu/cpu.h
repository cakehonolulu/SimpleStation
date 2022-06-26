#ifndef CPU_H
#define CPU_H

#include <simplestation.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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

// Internal defines
#define REGS (m_simplestation->m_cpu->m_registers)
#define INSTRUCTION ((uint32_t) (m_simplestation->m_cpu->m_opcode >> 26) & 63)

// Current immediate address (Opcode Bits [16:0])
#define IMMDT ((uint32_t) (m_simplestation->m_cpu->m_opcode & 0xFFFF))

// Current signed immediate address (Opcode Bits [16:0])
#define SIMMDT ((uint32_t) ((int16_t) (m_simplestation->m_cpu->m_opcode & 0xFFFF)))

// Current subfunction (Opcode Bits [10:6])
#define SUB ((uint32_t) (m_simplestation->m_cpu->m_opcode & 0x3F))

// Current shift immediate (Opcode Bits [10:6])
#define SHIFT ((uint32_t) ((m_simplestation->m_cpu->m_opcode >> 6) & 0x1F))

// Current jump immediate (Opcode Bits [10:6])
#define JIMMDT ((uint32_t) (m_simplestation->m_cpu->m_opcode & 0x3FFFFFF))

// Operand Register (Opcode Bits [25:21])
#define REGIDX_S ((uint32_t) ((m_simplestation->m_cpu->m_opcode >> 21) & 0x1F))

// Operand Register (Opcode Bits [20:16])
#define REGIDX_T ((uint32_t) ((m_simplestation->m_cpu->m_opcode >> 16) & 0x1F))

// Recieving Register (Opcode Bits [15:11])
#define REGIDX_D ((uint32_t) ((m_simplestation->m_cpu->m_opcode >> 11) & 0x1F))

// Register defines
#define PC (m_simplestation->m_cpu->m_pc)
#define HI (m_simplestation->m_cpu->m_hi)
#define LO (m_simplestation->m_cpu->m_lo)

#define ZERO (m_simplestation->m_cpu->m_registers[0])
#define AT (m_simplestation->m_cpu->m_registers[1])
#define V0 (m_simplestation->m_cpu->m_registers[2])
#define V1 (m_simplestation->m_cpu->m_registers[3])
#define A0 (m_simplestation->m_cpu->m_registers[4])
#define A1 (m_simplestation->m_cpu->m_registers[5])
#define A2 (m_simplestation->m_cpu->m_registers[6])
#define A3 (m_simplestation->m_cpu->m_registers[7])
#define T0 (m_simplestation->m_cpu->m_registers[8])
#define T1 (m_simplestation->m_cpu->m_registers[9])
#define T2 (m_simplestation->m_cpu->m_registers[10])
#define T3 (m_simplestation->m_cpu->m_registers[11])
#define T4 (m_simplestation->m_cpu->m_registers[12])
#define T5 (m_simplestation->m_cpu->m_registers[13])
#define T6 (m_simplestation->m_cpu->m_registers[14])
#define T7 (m_simplestation->m_cpu->m_registers[15])
#define S0 (m_simplestation->m_cpu->m_registers[16])
#define S1 (m_simplestation->m_cpu->m_registers[17])
#define S2 (m_simplestation->m_cpu->m_registers[18])
#define S3 (m_simplestation->m_cpu->m_registers[19])
#define S4 (m_simplestation->m_cpu->m_registers[20])
#define S5 (m_simplestation->m_cpu->m_registers[21])
#define S6 (m_simplestation->m_cpu->m_registers[22])
#define S7 (m_simplestation->m_cpu->m_registers[23])
#define T8 (m_simplestation->m_cpu->m_registers[24])
#define T9 (m_simplestation->m_cpu->m_registers[25])
#define K0 (m_simplestation->m_cpu->m_registers[26])
#define K1 (m_simplestation->m_cpu->m_registers[27])
#define GP (m_simplestation->m_cpu->m_registers[28])
#define SP (m_simplestation->m_cpu->m_registers[29])
#define FP (m_simplestation->m_cpu->m_registers[30])
#define RA (m_simplestation->m_cpu->m_registers[31])

/* Function definitions */
uint8_t m_cpu_init(m_simplestation_state *m_simplestation);
void m_cpu_fde(m_simplestation_state *m_simplestation);
void m_cpu_exit(m_simplestation_state *m_simplestation);
bool m_cpu_check_add_overflow(int32_t m_a, int32_t m_b);

#endif /* CPU_H */