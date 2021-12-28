#include <cpu/cpu.h>

// Declare a pointer to the CPU state structure
m_mips_r3000a_t *m_cpu;

// Current opcode
uint32_t m_opcode = 0;

// Current instruction (Opcode Bits [31:26])
unsigned _BitInt(5) m_instruction = 0;

// Current register index (Opcode Bits [20:16])
unsigned _BitInt(4) m_regidx = 0;

// Current immediate address (Opcode Bits [16:0])
unsigned _BitInt(16) m_immediate = 0;

// Function to initialize the CPU state
void m_cpu_init()
{
	// Malloc the CPU-state struct
	m_cpu = (m_mips_r3000a_t*) malloc(sizeof(m_mips_r3000a_t));

	// Check for malloc completion
	if (!m_cpu)
	{
		printf("Simplestation: Couldn't allocate CPU state struct, exiting...\n");
		exit(EXIT_FAILURE);
	}
#ifdef DEBUG_CPU
	else
	{
		printf("[CPU] Allocated CPU structure!\n");
	}
#endif

	// Point Program Counter to the initial BIOS address
	PC = 0xbfc00000;

	// High register to 0
	HI = 0;

	// Low register to 0
	LO = 0;

	// Set all registers to 0
	for (uint8_t m_regs = 0; m_regs < (M_R3000_REGISTERS - 1); m_regs++)
	{
		m_cpu->m_registers[m_regs] = 0;
	}
}

// Function to free the CPU struct after end-of-emulation
void m_cpu_exit()
{
	free(m_cpu);

#ifdef DEBUG_CPU
	printf("[CPU] Freed CPU structure!\n");
#endif
}

void m_cpu_fetch()
{
	/* Fetch cycle */
	m_opcode = READ32_BIOS(PC);
}

void m_cpu_decode()
{
	/*
		Opcode decoding:

		1st *Obtain the instruction
		2nd *Obtain the register index
		3rd *Obtain the immediate value
	*/
	
	// Instruction
	m_instruction = m_opcode >> 26;

	// Register index
	m_regidx = ((m_opcode >> 16) & 0xff);

	// Immediate
	m_immediate = (m_opcode & 0xffff);
}

void m_cpu_execute()
{
	printf("Opcode: 0x%06X, instr: 0x%x, regidx: 0x%x, imm: 0x%x\n", (uint32_t) m_opcode, (uint32_t) m_instruction, (uint32_t) m_regidx, (uint32_t) m_immediate);

	// Check if the instruction is implemented
	if (m_psx_instrs[m_instruction].m_funct == NULL)
	{
		printf("Unimplemented Opcode 0x%X\n", (uint32_t) m_instruction);
		m_printregs();
		m_bios_close();
		m_cpu_exit();
		exit(EXIT_FAILURE);
	}
	else
	{
		// Execute the instruction
		((void (*)(void))m_psx_instrs[m_instruction].m_funct)();
	}
}