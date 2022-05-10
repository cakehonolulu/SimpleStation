#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cpu/cpu.h>
#include <cpu/cop0.h>
#include <memory/memory.h>
#include <stdint.h>
#include <stdlib.h>

extern struct m_corewave_cw33300_instrs {
	const char *m_instr;
	void *m_funct;
} const m_psx_instrs[0x3F];

extern struct m_corewave_cw33300_extended_00_instrs {
	const char *m_instr;
	void *m_funct;
} const m_psx_extended_00[0x3F];

extern struct m_corewave_cw33300_cop0_instrs {
	const char *m_instr;
	void *m_funct;
} const m_psx_cop0[0x5];

/* Instructions */

// 0x00
void m_exp(m_simplestation_state *m_simplestation);

// 0x00 -> 0x00
void m_sll(m_simplestation_state *m_simplestation);

// 0x00 -> 0x25
void m_or(m_simplestation_state *m_simplestation);

// 0x02
void m_j(m_simplestation_state *m_simplestation);

// 0x05
void m_bne(m_simplestation_state *m_simplestation);

// 0x08
void m_addi(m_simplestation_state *m_simplestation);

// 0x09
void m_addiu(m_simplestation_state *m_simplestation);

// 0x10
void m_cop0(m_simplestation_state *m_simplestation);

// 0x0D
void m_ori(m_simplestation_state *m_simplestation);

// 0x0F
void m_lui(m_simplestation_state *m_simplestation);

// 0x23
void m_lw(m_simplestation_state *m_simplestation);

// 0x2B
void m_sw(m_simplestation_state *m_simplestation);

#endif /* INSTRUCTIONS_H */