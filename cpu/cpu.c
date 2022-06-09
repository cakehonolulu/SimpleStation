
#include <memory/memory.h>
#include <cpu/instructions.h>
#include <debugger/debugger.h>

// Function to initialize the CPU state
uint8_t m_cpu_init(m_simplestation_state *m_simplestation)
{
	// Return value
	uint8_t m_return = 0;

	// Malloc the CPU-state struct
	m_simplestation->m_cpu = (m_mips_r3000a_t*) malloc(sizeof(m_mips_r3000a_t));

	// Check if it has been allocated
	if (m_simplestation->m_cpu)
	{
#ifdef DEBUG_CPU
			printf("[CPU] init: Allocated CPU structure!\n");
#endif
		// Check if COP0 initialization succeeded
		if (m_cpu_cop0_init(m_simplestation) == 0)
		{
#ifdef DEBUG_CPU
			printf("[CPU] init: Allocated COP0 structure!\n");
#endif

			// Point Program Counter to the initial BIOS address
			PC = 0xBFC00000;

			// High register to 0
			HI = 0;

			// Low register to 0
			LO = 0;

			// Set current opcode to 0
			m_simplestation->m_cpu->m_opcode = 0;

			// Set next opcode in the pipeline to 0
			m_simplestation->m_cpu->m_next_opcode = 0;

			// Set all registers to 0
			for (uint8_t m_regs = 0; m_regs < M_R3000_REGISTERS; m_regs++)
			{
				m_simplestation->m_cpu->m_registers[m_regs] = 0;
			}

			// Set the CPU state to 'ON'
			m_simplestation->m_cpu_state = ON;
		}
		else
		{
			printf(RED "[CPU] init: Couldn't allocate CPU COP0 state struct, exiting...\n" NORMAL);
			m_return = 1;
		}
	}
	else
	{
		printf(RED "[CPU] init: Couldn't allocate CPU state struct, exiting...\n" NORMAL);
		m_return = 1;
	}

	return m_return;
}

// Function to free the CPU struct after end-of-emulation
void m_cpu_exit(m_simplestation_state *m_simplestation)
{
	// Clear the COP0 structures
	m_cpu_cop0_exit(m_simplestation);
	
	// Free the CPU struct if-and-only-if has been allocated
	if (m_simplestation->m_cpu)
	{
		free(m_simplestation->m_cpu);
	}

#ifdef DEBUG_CPU
	printf("[CPU] exit: Freed CPU structure!\n");
#endif
}

void m_cpu_fde(m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cpu->m_opcode = m_simplestation->m_cpu->m_next_opcode;

	/* Fetch cycle */
	m_simplestation->m_cpu->m_next_opcode = m_memory_read((PC), dword, m_simplestation);
	
	// Increment Program Counter by 4
	PC += 4;

	// Check if the instruction is implemented
	if (m_psx_instrs[INSTRUCTION].m_funct == NULL)
	{
		printf(RED "[CPU] fde: Unimplemented Instruction 0x%02X (Opcode: 0x%X)\n" NORMAL, INSTRUCTION, m_simplestation->m_cpu->m_opcode);
		m_printregs(m_simplestation);
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		// Execute the instruction
		((void (*) (m_simplestation_state *m_simplestation))m_psx_instrs[INSTRUCTION].m_funct)(m_simplestation);
	}
}