#ifndef OPCODES_H
#define OPCODES_H

#include <cpu/cop0.h>
#include <memory/memory.h>
#include <stdint.h>
#include <stdlib.h>

extern struct m_opc_names {
	const char *m_instr;
	void *m_funct;
} const m_psx_instrs_opcodes[0x3F];

extern struct m_opc_ext_names {
	const char *m_instr;
	void *m_funct;
} const m_psx_extended_00_opcodes[0x3F];

extern struct m_opc_cop0_names {
	const char *m_instr;
	void *m_funct;
} const m_psx_cop0_opcodes[0x11];

/* Instructions */

// COP0
void p_mfc0(m_simplestation_state *m_simplestation);

void p_mtc0(m_simplestation_state *m_simplestation);

void p_rfe(m_simplestation_state *m_simplestation);

// Exception handler
void p_exception(m_exc_types m_exception, m_simplestation_state *m_simplestation);

// 0x00
void p_exp(m_simplestation_state *m_simplestation);

// 0x00 -> 0x00
void p_sll(m_simplestation_state *m_simplestation);

// 0x00 -> 0x02
void p_srl(m_simplestation_state *m_simplestation);

// 0x00 -> 0x03
void p_sra(m_simplestation_state *m_simplestation);

// 0x00 -> 0x04
void p_sllv(m_simplestation_state *m_simplestation);

// 0x00 -> 0x08
void p_jr(m_simplestation_state *m_simplestation);

// 0x00 -> 0x09
void p_jalr(m_simplestation_state *m_simplestation);

// 0x00 -> 0x0C
void p_syscall(m_simplestation_state *m_simplestation);

// 0x00 -> 0x10
void p_mfhi(m_simplestation_state *m_simplestation);

// 0x00 -> 0x11
void p_mthi(m_simplestation_state *m_simplestation);

// 0x00 -> 0x12
void p_mflo(m_simplestation_state *m_simplestation);

// 0x00 -> 0x13
void p_mtlo(m_simplestation_state *m_simplestation);

// 0x00 -> 0x1A
void p_div(m_simplestation_state *m_simplestation);

// 0x00 -> 0x1B
void p_divu(m_simplestation_state *m_simplestation);

// 0x00 -> 0x20
void p_add(m_simplestation_state *m_simplestation);

// 0x00 -> 0x21
void p_addu(m_simplestation_state *m_simplestation);

// 0x00 -> 0x23
void p_subu(m_simplestation_state *m_simplestation);

// 0x00 -> 0x24
void p_and(m_simplestation_state *m_simplestation);

// 0x00 -> 0x25
void p_or(m_simplestation_state *m_simplestation);

// 0x00 -> 0x2A
void p_slt(m_simplestation_state *m_simplestation);

// 0x00 -> 0x2B
void p_sltu(m_simplestation_state *m_simplestation);

// 0x01
void p_bxx(m_simplestation_state *m_simplestation);

// 0x02
void p_j(m_simplestation_state *m_simplestation);

// 0x03
void p_jal(m_simplestation_state *m_simplestation);

// 0x04
void p_beq(m_simplestation_state *m_simplestation);

// 0x05
void p_bne(m_simplestation_state *m_simplestation);

// 0x06
void p_blez(m_simplestation_state *m_simplestation);

// 0x07
void p_bgtz(m_simplestation_state *m_simplestation);

// 0x08
void p_addi(m_simplestation_state *m_simplestation);

// 0x09
void p_addiu(m_simplestation_state *m_simplestation);

// 0x0A
void p_slti(m_simplestation_state *m_simplestation);

// 0x0B
void p_sltiu(m_simplestation_state *m_simplestation);

// 0x10
void p_cop0(m_simplestation_state *m_simplestation);

// 0x0C
void p_andi(m_simplestation_state *m_simplestation);

// 0x0D
void p_ori(m_simplestation_state *m_simplestation);

// 0x0F
void p_lui(m_simplestation_state *m_simplestation);

// 0x20
void p_lb(m_simplestation_state *m_simplestation);

// 0x23
void p_lw(m_simplestation_state *m_simplestation);

// 0x24
void p_lbu(m_simplestation_state *m_simplestation);

// 0x25
void p_lhu(m_simplestation_state *m_simplestation);

// 0x28
void p_sb(m_simplestation_state *m_simplestation);

// 0x29
void p_sh(m_simplestation_state *m_simplestation);

// 0x2B
void p_sw(m_simplestation_state *m_simplestation);

#endif /* OPCODES_H */