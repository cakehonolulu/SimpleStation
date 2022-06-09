#ifndef COP0_H
#define COP0_H

#include <cpu/instructions.h>
#include <cpu/cpu.h>
#include <stdint.h>

#define COP0_REGS (m_simplestation->m_cpu_cop0->m_registers)
#define COP0_STATUS_REGISTER COP0_REGS[12]

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

uint8_t m_cpu_cop0_init(m_simplestation_state *m_simplestation);
void m_cpu_cop0_exit(m_simplestation_state *m_simplestation);

// 0x04
void m_mtc0(m_simplestation_state *m_simplestation);

#endif /* COP0_H */