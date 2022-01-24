#ifndef COP0_H
#define COP0_H

#include <cpu/instructions.h>
#include <cpu/cpu.h>
#include <stdint.h>

#define M_R3000_COP0_REGISTERS 32

typedef struct m_corewave_cw33300_cop0
{
	uint32_t m_registers[M_R3000_COP0_REGISTERS];

} m_mips_r3000a_cop0_t;

extern m_mips_r3000a_cop0_t *m_cpu_cop0;

#define COP0_REGS (m_cpu_cop0->m_registers)

static const char *m_cop0_regnames[] = {
	"cop0_0", "cop0_1", "cop0_2",
	"cop0_3",
	"cop0_4",
	"cop0_5",
	"cop0_6",
	"cop0_7",
	"cop0_8",
	"cop0_9",
	"cop0_10",
	"cop0_11",
	"cop0_12",
	"cop0_13",
	"cop0_14",
	"cop0_15",
	"cop0_16", "cop0_17", "cop0_18", "cop0_19", "cop0_20", "cop0_21",
	"cop0_22", "cop0_23", "cop0_24", "cop0_25", "cop0_26", "cop0_27",
	"cop0_28", "cop0_29", "cop0_30", "cop0_31"
};

void m_cpu_cop0_init();
void m_cpu_cop0_exit();

// 0x04
void m_mtc0();

#endif /* COP0_H */