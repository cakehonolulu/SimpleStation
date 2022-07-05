#ifndef COP0_H
#define COP0_H

#include <cpu/instructions.h>
#include <cpu/cpu.h>
#include <ui/termcolour.h>
#include <stdint.h>

#define COP0_REGS (m_simplestation->m_cpu_cop0->m_registers)
#define COP0_STATUS_REGISTER COP0_REGS[12]

// Register names
extern const char *m_cop0_regnames[];

/* Function definitions */
uint8_t m_cpu_cop0_init(m_simplestation_state *m_simplestation);
void m_cpu_cop0_exit(m_simplestation_state *m_simplestation);

// 0x00
void m_mfc0(m_simplestation_state *m_simplestation);

// 0x04
void m_mtc0(m_simplestation_state *m_simplestation);

#endif /* COP0_H */