
#include <memory/memory.h>
#include <cpu/instructions.h>
#include <debugger/debugger.h>
#include <limits.h>

// Register names
const char *m_cpu_regnames[] = {
	"zr",
	"at",
	"v0", "v1",
	"a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9",
	"k0", "k1",
	"gp",
	"sp",
	"fp",
	"ra"
};

// Function to initialize the CPU state
uint8_t m_cpu_init(m_simplestation_state *m_simplestation)
{
#ifdef DEBUG_CPU
	// Number of implemented instructions
	uint8_t m_cpu_total_impl_opcodes = 0, m_cpu_impl_reg_opcodes = 0, m_cpu_impl_ext_opcodes = 0, m_cpu_impl_cop0_opcodes = 0, i;
#endif

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

#ifdef DEBUG_CPU
			for (i = 0; i < 0x3F; i++)
			{
				if (m_psx_instrs[i].m_funct != NULL)
				{
					m_cpu_total_impl_opcodes++;
					m_cpu_impl_reg_opcodes++;
				}
			}

			for (i = 0; i < 0x3F; i++)
			{
				if (m_psx_extended_00[i].m_funct != NULL)
				{
					m_cpu_total_impl_opcodes++;
					m_cpu_impl_ext_opcodes++;
				}
			}

			for (i = 0; i < 0x5; i++)
			{
				if (m_psx_cop0[i].m_funct != NULL)
				{
					m_cpu_total_impl_opcodes++;
					m_cpu_impl_cop0_opcodes++;
				}
			}

			printf("[CPU] init: Implemented regular opcodes: %d (Out of 40), extended opcodes: %d (Out of 28), cop0 opcodes: %d (Out of 14)\n",
					m_cpu_impl_reg_opcodes, m_cpu_impl_ext_opcodes, m_cpu_impl_cop0_opcodes);
			printf("[CPU] init: Total implemented opcodes: %d (Out of 82)...\n", m_cpu_total_impl_opcodes);
			printf("[CPU] init: CPU Subsystem is %d%%~ done\n", ((m_cpu_total_impl_opcodes * 82) / 100));	
#endif
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
		printf(RED "[CPU] fde: Unimplemented Instruction 0x%02X (Full Opcode: 0x%X)\n" NORMAL, INSTRUCTION, m_simplestation->m_cpu->m_opcode);
		m_printregs(m_simplestation);
		m_simplestation_exit(m_simplestation, 1);
	}
	else
	{
		// Execute the instruction
		((void (*) (m_simplestation_state *m_simplestation))m_psx_instrs[INSTRUCTION].m_funct)(m_simplestation);
	}
}

bool m_cpu_check_signed_addition(int32_t m_first_num, int32_t m_second_num)
{
	bool m_result = false;

#ifndef __clang__
	if (CHECK_ADD_OVERFLOW(m_first_num, m_second_num))
#else
	int32_t m_number;

	if (CHECK_ADD_OVERFLOW(m_first_num, m_second_num, &m_number))
#endif
	{
		m_result = true;
	}

	return m_result;
}
