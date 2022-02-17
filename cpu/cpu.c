#include <cpu/cpu.h>

// Declare a pointer to the CPU state structure
m_mips_r3000a_t *m_cpu;

// Current opcode
uint32_t m_opcode = 0;

uint32_t m_next_opcode = 0;

// Function to initialize the CPU state
void m_cpu_init()
{
	// Malloc the CPU-state struct
	m_cpu = (m_mips_r3000a_t*) malloc(sizeof(m_mips_r3000a_t));

	m_cpu_cop0_init();

	// Check for malloc completion
	if (!m_cpu)
	{
		printf(RED "[CPU] init: Couldn't allocate CPU state struct, exiting...\n" NORMAL);
		exit(EXIT_FAILURE);
	}
#ifdef DEBUG_CPU
	else
	{
		printf("[CPU] init: Allocated CPU structure!\n");
	}
#endif

	// Point Program Counter to the initial BIOS address
	PC = 0xBFC00000;

	// High register to 0
	HI = 0;

	// Low register to 0
	LO = 0;

	// Set all registers to 0
	for (uint8_t m_regs = 0; m_regs < M_R3000_REGISTERS; m_regs++)
	{
		m_cpu->m_registers[m_regs] = 0;
	}

	m_simplestation.m_cpu_state = ON;
}

// Function to free the CPU struct after end-of-emulation
void m_cpu_exit()
{
	m_cpu_cop0_exit();
	
	free(m_cpu);

#ifdef DEBUG_CPU
	printf("[CPU] exit: Freed CPU structure!\n");
#endif
}

void m_cpu_fde()
{
	m_opcode = m_next_opcode;

	/* Fetch cycle */
	m_next_opcode = READ32_BIOS(PC);
	
	// Increment Program Counter by 4
	PC += 4;

	// Check if the instruction is implemented
	if (m_psx_instrs[INSTRUCTION].m_funct == NULL)
	{
		printf(RED "[CPU] fde: Unimplemented Instruction 0x%02X (Opcode: 0x%X)\n" NORMAL, INSTRUCTION, m_opcode);
		m_printregs();
		m_simplestation_exit();
	}
	else
	{
		// Execute the instruction
		((void (*)(void))m_psx_instrs[INSTRUCTION].m_funct)();
	}
}