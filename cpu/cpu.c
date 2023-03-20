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
	uint8_t m_cpu_total_impl_opcodes = 1, m_cpu_impl_reg_opcodes = 0, m_cpu_impl_ext_opcodes = 0, i;
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

			gte_init(m_simplestation);

			// Point Program Counter to the initial BIOS address
			PC = 0;
			NXT_PC = 0xBFC00000;
			m_simplestation->m_cpu->m_pc_cur = 0;

			// High register to 0
			HI = 0;

			// Low register to 0
			LO = 0;

			// Set current opcode to 0
			m_simplestation->m_cpu->m_opcode = 0;

			// Set all registers to 0
			for (uint8_t m_regs = 0; m_regs < M_R3000_REGISTERS; m_regs++)
			{
				m_simplestation->m_cpu->m_registers[m_regs] = 0;
			}

    		m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register = 0;
			m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value = 0;

			m_simplestation->m_cpu->m_branch = false;
			m_simplestation->m_cpu->m_delay = false;
			m_simplestation->m_cpu->m_pre_ds_pc = 0;

			m_simplestation->m_cond = false;

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

			printf("[CPU] init: Implemented regular opcodes: %d (Out of 40), extended opcodes: %d (Out of 28)\n",
					m_cpu_impl_reg_opcodes, m_cpu_impl_ext_opcodes);
			printf("[CPU] init: Total implemented opcodes: %d (Out of 69)...\n", m_cpu_total_impl_opcodes);
			printf("[CPU] init: CPU Subsystem is %d%%~ done\n", ((m_cpu_total_impl_opcodes * 100) / 69));	
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
	gte_exit(m_simplestation);

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
#ifdef DEBUG_INSTRUCTIONS
	if (NXT_PC > (PC + 4))
	{
		printf("\n" GREEN "[DS]" NORMAL " PC: 0x%08X\n", PC);
	}
	else
	{
		printf("\nPC: 0x%08X\n", PC);
	}
#endif

	if (m_simplestation->m_tty == true)
	{
		// Check if next PC value equals 0xB0
		if (NXT_PC == 0xB0)
		{
			// Read Current PC
			uint32_t m_is_addiu = m_memory_read(PC, dword, m_simplestation);
			
			// Check if opcode is ADDIU (0x09) 
			if (((uint32_t) (m_is_addiu >> 26) & 63) == 0x09)
			{
				// Check if it's signed immediate value equals 0x3D
				if (((uint32_t) ((int16_t) (m_is_addiu & 0xFFFF))) == 0x3D)
				{
					// Character to print is at the LSB of the A0 reguster
					printf("%c", (char) REGS[4]);
				}
			}
		}
	}

	if (m_simplestation->m_sideload == true)
	{	
		if (PC == 0xBFC06FF0)
		{
			EXEheader_t* exe = loadFile( m_simplestation->exename );
			
			m_simplestation->m_cpu->m_opcode = 0;

			PC = exe->pc - 4;
			NXT_PC = PC + 4;
			//REGS[28] = exe->gp;
			//REGS[29] = exe->sp + exe->spOffset;
			//REGS[30] = REGS[29];
			
			for( int i = 0; i < exe->size; i++ ) {
				m_memory_write( exe->dst + i, exe->data[i] , byte , m_simplestation);
			}
		}
	}


	/* Fetch cycle */
	m_simplestation->m_cpu->m_opcode = m_memory_read((PC), dword, m_simplestation);

#ifdef DEBUG_INSTRUCTIONS
	printf("Opcode: 0x%08X\nNext Opcode: 0x%08X\n", m_simplestation->m_cpu->m_opcode, m_simplestation->m_cpu->m_next_opcode);
#endif

	m_simplestation->m_cpu->m_pc_cur = PC;

	PC = NXT_PC;
	
	if ((PC % 4) != 0)
	{
		m_exc_types m_exc = load_error;
		m_exception(m_exc, m_simplestation);
		return;
	}

	// Increment Program Counter by 4
	NXT_PC += 4;

	m_simplestation->m_cpu->m_delay = m_simplestation->m_cpu->m_branch;
	m_simplestation->m_cpu->m_branch = false;

#ifndef GDBSTUB_SUPPORT
	if ((PC - 4) == m_simplestation->m_breakpoint)
	{
		if (m_simplestation->m_debugger)
		{
			m_debugger(m_simplestation);
			m_simplestation_exit(m_simplestation, 1);
		}
		else
		{
			m_simplestation_exit(m_simplestation, 1);
		}
	}
#endif

	if (m_interrupts_pending(m_simplestation))
	{
        m_exception(interrupt, m_simplestation);
		return;
    }

	// Check if the instruction is implemented
	if (m_psx_instrs[INSTRUCTION].m_funct)
	{
		// Set $zr register to 0
		REGS[0] = 0;

		// Execute the instruction
		((void (*) (m_simplestation_state *m_simplestation))m_psx_instrs[INSTRUCTION].m_funct)(m_simplestation);

		if (m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register)
		{
			m_cpu_delay_slot_handler(m_simplestation);
		}
	}
	else
	{
		printf(RED "[CPU] fde: Illegal Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, REGIDX_S, m_simplestation->m_cpu->m_opcode);
		m_exc_types m_exc = illegal;
		m_exception(m_exc, m_simplestation);
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

bool m_cpu_check_signed_subtraction(int32_t m_first_num, int32_t m_second_num)
{
	bool m_result = false;

#ifndef __clang__
	if (CHECK_SUB_OVERFLOW(m_first_num, m_second_num))
#else
	int32_t m_number;

	if (CHECK_SUB_OVERFLOW(m_first_num, m_second_num, &m_number))
#endif
	{
		m_result = true;
	}

	return m_result;
}

void m_cpu_branch(int32_t m_offset, m_simplestation_state *m_simplestation)
{
	m_offset <<= 2;

	NXT_PC += m_offset;
	NXT_PC -= 4;
	m_simplestation->m_cpu->m_branch = true;
}

void m_cpu_delay_slot_handler(m_simplestation_state *m_simplestation)
{
	switch (m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_size)
	{
		case byte:
			REGS[m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register] = (uint8_t) m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value;
			break;

		case word:
			REGS[m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register] = (uint16_t) m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value;
			break;

		case dword:
			REGS[m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register] = (uint32_t) m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value;
			break;
	}

    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value = 0;
    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register = 0;
	m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_size = byte;
}

void m_cpu_load_delay_enqueue_byte(uint8_t m_register, uint8_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register = m_register;
    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value = m_value;
	m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_size = byte;
}

void m_cpu_load_delay_enqueue_word(uint8_t m_register, uint16_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register = m_register;
    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value = m_value;
	m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_size = word;
}

void m_cpu_load_delay_enqueue_dword(uint8_t m_register, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_register = m_register;
    m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_value = m_value;
	m_simplestation->m_cpu->m_cpu_delayed_memory_load.m_size = dword;
}
