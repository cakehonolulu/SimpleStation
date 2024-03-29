#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cpu/cop0.h>
#include <memory/memory.h>
#include <stdint.h>
#include <stdlib.h>

extern struct m_corewave_cw33300_instrs {
	void *m_funct;
} const m_psx_instrs[0x3F];

extern struct m_corewave_cw33300_extended_00_instrs {
	void *m_funct;
} const m_psx_extended_00[0x3F];

extern struct m_corewave_cw33300_cop0_instrs {
	void *m_funct;
} const m_psx_cop0[0x11];

#define ILLEGAL NULL

/* Instructions */

// Exception handler
void m_exception(m_exc_types m_exception, m_simplestation_state *m_simplestation);

// 0x00
void m_exp(m_simplestation_state *m_simplestation);

// 0x00 -> 0x00
void m_sll(m_simplestation_state *m_simplestation);

// 0x00 -> 0x02
void m_srl(m_simplestation_state *m_simplestation);

// 0x00 -> 0x03
void m_sra(m_simplestation_state *m_simplestation);

// 0x00 -> 0x04
void m_sllv(m_simplestation_state *m_simplestation);

// 0x00 -> 0x06
void m_srlv(m_simplestation_state *m_simplestation);

// 0x00 -> 0x07
void m_srav(m_simplestation_state *m_simplestation);

// 0x00 -> 0x08
void m_jr(m_simplestation_state *m_simplestation);

// 0x00 -> 0x09
void m_jalr(m_simplestation_state *m_simplestation);

// 0x00 -> 0x0C
void m_syscall(m_simplestation_state *m_simplestation);

// 0x00 -> 0x0C
void m_break(m_simplestation_state *m_simplestation);

// 0x00 -> 0x10
void m_mfhi(m_simplestation_state *m_simplestation);

// 0x00 -> 0x11
void m_mthi(m_simplestation_state *m_simplestation);

// 0x00 -> 0x12
void m_mflo(m_simplestation_state *m_simplestation);

// 0x00 -> 0x13
void m_mtlo(m_simplestation_state *m_simplestation);

// 0x00 -> 0x18
void m_mult(m_simplestation_state *m_simplestation);

// 0x00 -> 0x19
void m_multu(m_simplestation_state *m_simplestation);

// 0x00 -> 0x1A
void m_div(m_simplestation_state *m_simplestation);

// 0x00 -> 0x1B
void m_divu(m_simplestation_state *m_simplestation);

// 0x00 -> 0x20
void m_add(m_simplestation_state *m_simplestation);

// 0x00 -> 0x21
void m_addu(m_simplestation_state *m_simplestation);

// 0x00 -> 0x22
void m_sub(m_simplestation_state *m_simplestation);

// 0x00 -> 0x23
void m_subu(m_simplestation_state *m_simplestation);

// 0x00 -> 0x24
void m_and(m_simplestation_state *m_simplestation);

// 0x00 -> 0x25
void m_or(m_simplestation_state *m_simplestation);

// 0x00 -> 0x26
void m_xor(m_simplestation_state *m_simplestation);

// 0x00 -> 0x27
void m_nor(m_simplestation_state *m_simplestation);

// 0x00 -> 0x2A
void m_slt(m_simplestation_state *m_simplestation);

// 0x00 -> 0x2B
void m_sltu(m_simplestation_state *m_simplestation);

// 0x01
void m_bxx(m_simplestation_state *m_simplestation);

// 0x02
void m_j(m_simplestation_state *m_simplestation);

// 0x03
void m_jal(m_simplestation_state *m_simplestation);

// 0x04
void m_beq(m_simplestation_state *m_simplestation);

// 0x05
void m_bne(m_simplestation_state *m_simplestation);

// 0x06
void m_blez(m_simplestation_state *m_simplestation);

// 0x07
void m_bgtz(m_simplestation_state *m_simplestation);

// 0x08
void m_addi(m_simplestation_state *m_simplestation);

// 0x09
void m_addiu(m_simplestation_state *m_simplestation);

// 0x0A
void m_slti(m_simplestation_state *m_simplestation);

// 0x0B
void m_sltiu(m_simplestation_state *m_simplestation);

// 0x10
void m_cop0(m_simplestation_state *m_simplestation);

// 0x11
void m_cop1(m_simplestation_state *m_simplestation);

// 0x12
void m_cop2(m_simplestation_state *m_simplestation);

// 0x13
void m_cop3(m_simplestation_state *m_simplestation);

// 0x0C
void m_andi(m_simplestation_state *m_simplestation);

// 0x0D
void m_ori(m_simplestation_state *m_simplestation);

// 0x0E
void m_xori(m_simplestation_state *m_simplestation);

// 0x0F
void m_lui(m_simplestation_state *m_simplestation);

// 0x20
void m_lb(m_simplestation_state *m_simplestation);

// 0x21
void m_lh(m_simplestation_state *m_simplestation);

// 0x22
void m_lwl(m_simplestation_state *m_simplestation);

// 0x23
void m_lw(m_simplestation_state *m_simplestation);

// 0x24
void m_lbu(m_simplestation_state *m_simplestation);

// 0x25
void m_lhu(m_simplestation_state *m_simplestation);

// 0x26
void m_lwr(m_simplestation_state *m_simplestation);

// 0x28
void m_sb(m_simplestation_state *m_simplestation);

// 0x29
void m_sh(m_simplestation_state *m_simplestation);

// 0x2A
void m_swl(m_simplestation_state *m_simplestation);

// 0x2B
void m_sw(m_simplestation_state *m_simplestation);

// 0x2E
void m_swr(m_simplestation_state *m_simplestation);

void m_lwc0(m_simplestation_state *m_simplestation);

void m_lwc1(m_simplestation_state *m_simplestation);

void m_lwc2(m_simplestation_state *m_simplestation);

void m_lwc3(m_simplestation_state *m_simplestation);

void m_swc0(m_simplestation_state *m_simplestation);

void m_swc1(m_simplestation_state *m_simplestation);

void m_swc2(m_simplestation_state *m_simplestation);

void m_swc3(m_simplestation_state *m_simplestation);

#endif /* INSTRUCTIONS_H */