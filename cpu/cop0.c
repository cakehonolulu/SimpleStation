#include <cpu/cop0.h>

// Register names
const char *m_cop0_regnames[] = {
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

uint8_t m_cpu_cop0_init(m_simplestation_state *m_simplestation)
{
	uint8_t m_return = 0;

	// Malloc the COP0-state struct
	m_simplestation->m_cpu_cop0 = (m_mips_r3000a_cop0_t*) malloc(sizeof(m_mips_r3000a_cop0_t));

	// Check if it has been allocated
	if (m_simplestation->m_cpu_cop0)
	{
#ifdef DEBUG_CPU_COP0
		printf("[CPU] Allocated CPU's Coprocessor 0 structure!\n");
#endif

		// Set all Coprocessor 0 Registers to 0
		for (uint8_t m_cop0_regs = 0; m_cop0_regs < M_R3000_COP0_REGISTERS; m_cop0_regs++)
		{
			m_simplestation->m_cpu_cop0->m_registers[m_cop0_regs] = 0;
		}
	}
	else
	{
		printf("Simplestation: Couldn't allocate CPU's Coprocessor 0 state struct, exiting...\n");
		m_return = 1;
	}

	return m_return;
}

void m_cpu_cop0_exit(m_simplestation_state *m_simplestation)
{
	// Free the COP0 struct if-and-only-if has been allocated
	if (m_simplestation->m_cpu_cop0 != NULL)
	{
		free(m_simplestation->m_cpu_cop0);	
	}
}

void m_mfc0(m_simplestation_state *m_simplestation)
{
    if (REGIDX_D == 3 || (REGIDX_D >= 5 && REGIDX_D <= 9) || (REGIDX_D >= 11 && REGIDX_D <= 15))
	{

#ifdef DEBUG_CPU_COP0
		if (REGIDX_D == 13)
		{

			printf(YELLOW "[COP0] mfc0: Read from 'Cause' register!\n" WHITE NORMAL);
		}
#endif

        m_cpu_load_delay_enqueue_dword(REGIDX_T, COP0_REGS[REGIDX_D], m_simplestation);
    }
    else
	{
		printf(RED "[COP0] mfc0: Unhandled read from COP0...\n");
		m_simplestation_exit(m_simplestation, 1);
    }
}

void m_mtc0(m_simplestation_state *m_simplestation)
{
	COP0_REGS[REGIDX_D] = REGS[REGIDX_T];
}

/*
    RFE (MIPS)

	Format:
    RFE

	Description:
	This instuction is not implemented on R4000 processors; use ERET instead.
	RFE restores the previous interrupt mask and Kernel/User-mode bits (IEp and KUp)
	of the Status register (SR) into the corresponding current status bits (IEc and KUc) and
	restores the old status bits (IEo and KUo) into the corresponding previous status bits
	(IEp and KUp). The old status bits remain unchanged.
	The architecture does not specify the operation of memory references associated with
	load/store instructions immediately prior to an RFE instruction. Normally, the RFE
	instruction follows in the delay slot of a JR (jump register)instruction to restore the PC.
	R2000/R3000/R6000
*/
void m_rfe(m_simplestation_state *m_simplestation)
{
	if ((m_simplestation->m_cpu->m_opcode & 0x3F) != 0b010000)
	{
		printf(RED "[COP0] rfe: Invalid COP0 instruction...! (0x%08X)\n", m_simplestation->m_cpu->m_opcode);
	}

	uint32_t mode = COP0_SR & 0x3f;
	COP0_SR &= ~0xf;
	COP0_SR |= mode >> 2;
}
