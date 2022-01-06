#include <memory/memory.h>

// PSX RAM Memory Buffer
int8_t *m_mem_ram;

// PSX BIOS Memory Buffer
int8_t *m_mem_bios;

// PSX Memory Map Walktable
uint32_t m_memory_map[] = {
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,		// KUSEG: 2048MB
	0x7FFFFFFF,											// KSEG0:  512MB
	0x1FFFFFFF,											// KSEG1:  512MB
	0xFFFFFFFF, 0xFFFFFFFF								// KSEG2: 1024MB
};

/*
	Function:
	m_memory_init

	Arguments:
	none

	Description:
	Initializes SimpleStation's Memory Subsystem.
*/
void m_memory_init()
{
	m_mem_ram = (int8_t *) malloc(sizeof(PSX_MEM));

	if (!m_mem_ram)
	{
		printf("Simplestation: Couldn't allocate PSX RAM Memory, exiting...");
	}
}

/*
	Function:
	m_memory_exit

	Arguments:
	none

	Description:
	Cleanly closes SimpleStation's Memory Subsystem.
*/
void m_memory_exit()
{
	// Free the RAM Buffer
	free(m_mem_ram);

	// Exit the BIOS Subsystem (Frees the BIOS Memory)
	m_bios_exit();
}

/*
	Function:
	m_memory_read_dword(uint32_t m_memory_address, int8_t *m_memory_source)

	Arguments:
	-> m_memory_address: Acts as the memory pointer where the value will be read.
	-> m_memory_source: The memory buffer where the value will be read at.

	Description:
	Reads a value at a determined memory address.
	m_memory_source determines the buffer where it should look the value at.
*/
uint32_t m_memory_read_dword(uint32_t m_memory_address, int8_t *m_memory_source)
{
	return *(uint32_t*)(m_memory_source + m_memory_address);
}

/*
	Function:
	m_memory_read(uint32_t m_memory_offset, m_memory_size m_size)

	Arguments:
	-> m_memory_offset: Offset where to look the value at

	Description:
	Returns a double-word (32-bit value) located at that memory offset
*/
uint32_t m_memory_read(uint32_t m_memory_offset, m_memory_size m_size)
{
	// Calculate the absolute memory address to read at
	uint32_t m_placeholder = m_memory_offset >> 29;
	uint32_t m_address = m_memory_offset & m_memory_map[m_placeholder];

	// Check for a read in BIOS area
	if (m_address < 0x1FC80000)
	{
		// Based on the m_size argument, select the memory size to be read
		switch (m_size)
		{
			case byte:
				break;

			case word:
				break;

			case dword:
				return m_memory_read_dword(m_address & 0x7FFFF, m_mem_bios);

			default:
				printf("[mem] Unknown Memory Read Size! (%d)\n", m_size);
				m_memory_exit();
				m_cpu_exit();
				exit(EXIT_FAILURE);
		}
	}
	else
	{
		printf("[mem] Region not implemented!\n");
		printf("Address: 0x%08X; Offset: 0x%08X\n", m_address, m_memory_offset);
		m_memory_exit();
		m_cpu_exit();
		exit(EXIT_FAILURE);
	}
}