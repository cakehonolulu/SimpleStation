#ifndef COP0_H
#define COP0_H

#include <cpu/instructions.h>
#include <stdint.h>

typedef struct m_corewave_cw33300_cop0
{
	uint32_t m_registers[1];

} m_mips_r3000a_cop0_t;

extern m_mips_r3000a_cop0_t *m_cpu_cop0;

void m_cpu_cop0_init();
void m_cpu_cop0_exit();

// 0x04
void m_mtc0();

#endif /* COP0_H */