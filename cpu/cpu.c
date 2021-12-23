#include <cpu/cpu.h>

// Declare a pointer to the CPU state structure
m_mips_r3000a_t *m_cpu;

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
	uint32_t m_opcode = READ32_BIOS(PC);

	printf("0x%x\n", m_opcode);
	PC += 4;
}

void m_cpu_decode()
{

}

void m_cpu_execute()
{
	
}