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
}

// Function to free the CPU struct after end-of-emulation
void m_cpu_exit()
{
	free(m_cpu);
}