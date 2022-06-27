#include <memory/memory.h>
#include <cpu/interrupts.h>
#include <cpu/bios.h>
#include <ui/termcolour.h>
#include <endian.h>
#include <stdlib.h>
#include <stdio.h>

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
uint8_t m_memory_init(m_simplestation_state *m_simplestation)
{
	uint8_t m_return = 0;

	// Allocate the basic memory struct
	m_simplestation->m_memory = (m_psx_memory_t *) malloc(sizeof(m_psx_memory_t));

	if (m_simplestation->m_memory)
	{
		// 2 MiB Total
		m_simplestation->m_memory->m_mem_ram = (int8_t *) malloc(sizeof(PSX_MEM));

		if (m_simplestation->m_memory->m_mem_ram)
		{
			// 1 KiB Total
			m_simplestation->m_memory->m_mem_scratchpad = (int8_t *) malloc(1 * KiB);

			if (m_simplestation->m_memory->m_mem_scratchpad)
			{
				// 0x20 Total (0x1F801020 - 0x1F801000)
				m_simplestation->m_memory->m_mem_memctl1 = (int8_t *) malloc(0x20);

				if (m_simplestation->m_memory->m_mem_memctl1)
				{
					m_simplestation->m_memory_state = ON;
				}
				else
				{
					printf("[MEM] init: Couldn't allocate PSX's MemCtl Registers, exiting...");
					m_return = 1;
				}
			}
			else
			{
				printf("[MEM] init: Couldn't allocate PSX's Scratchpad Memory, exiting...");
				m_return = 1;
			}
		}
		else
		{
			printf("[MEM] init: Couldn't allocate PSX RAM Memory, exiting...");
			m_return = 1;
		}
	}
	else
	{
		printf("[MEM] init: Couldn't allocate Memory Structure, exiting...");
		m_return = 1;
	}

	return m_return;
}

/*
	Function:
	m_memory_exit

	Arguments:
	none

	Description:
	Cleanly closes SimpleStation's Memory Subsystem.
*/
void m_memory_exit(m_simplestation_state *m_simplestation)
{
	if (m_simplestation->m_memory->m_mem_ram)
	{
		// Free the RAM Buffer
		free(m_simplestation->m_memory->m_mem_ram);
	}

	if (m_simplestation->m_memory->m_mem_scratchpad)
	{
		// Free the Scratchpad Buffer
		free(m_simplestation->m_memory->m_mem_scratchpad);
	}

	if (m_simplestation->m_memory->m_mem_memctl1)
	{
		// Free the Memory Control 1 Buffer
		free(m_simplestation->m_memory->m_mem_memctl1);
	}

	// Exit the BIOS Subsystem (Frees the BIOS Memory)
	m_bios_exit(m_simplestation);
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
#if __BYTE_ORDER == __LITTLE_ENDIAN
	/*
		If on LE, it's safe to assume we'll read the bytes in the order we want them.
		This reduces the function size by a large margin, and therefor, performs faster.
	*/
	return *(uint32_t*)(m_memory_source + (m_memory_address + 0));
#else
	/*
		Else, if on BE, use the legacy fallback method; it's clunkier and slower but works.
	*/
	uint8_t b0 = *(uint8_t*)(m_memory_source + (m_memory_address + 0));
	uint8_t b1 = *(uint8_t*)(m_memory_source + (m_memory_address + 1));
	uint8_t b2 = *(uint8_t*)(m_memory_source + (m_memory_address + 2));
	uint8_t b3 = *(uint8_t*)(m_memory_source + (m_memory_address + 3));

	return (b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
#endif
}

uint32_t m_memory_write_dword(uint32_t m_memory_address, uint32_t m_value, int8_t *m_memory_source)
{
	return *(uint32_t *) (m_memory_address + m_memory_source) = m_value;
}

/*
	Function:
	m_memory_read(uint32_t m_memory_offset)

	Arguments:
	-> m_memory_offset: Offset where to look the value at

	Description:
	Returns a double-word (32-bit value) located at that memory offset
*/
uint32_t m_memory_read(uint32_t m_memory_offset, m_simplestation_state *m_simplestation)
{
	uint32_t m_placeholder, m_address, m_return = 0;

	// PSX doesn't permit unaligned memory reads
	if (m_memory_offset % 4 != 0)
	{
		printf(RED "[MEM] read: Unaligned memory read! Exiting...\n" NORMAL);
		return m_simplestation_exit(m_simplestation, 1);
	}

	// Calculate the absolute memory address to read at
	m_placeholder = m_memory_offset >> 29;
	m_address = m_memory_offset & m_memory_map[m_placeholder];

	// Check for a read in BIOS area
	if (m_address == 0x1F801060)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] read: RAM_SIZE Register (Current Value: 0x%0X)\n" NORMAL, m_simplestation->m_memory->m_memory_ram_config_reg);
#endif
		m_return = m_simplestation->m_memory->m_memory_ram_config_reg;
	}
	else if ((0x1F800000 <= m_address) && (m_address < 0x1F800400))
	{
		m_return = m_memory_read_dword(m_address & 0x3FF, m_simplestation->m_memory->m_mem_scratchpad);
	}
	else if (m_address < 0x1F802000)
	{
		// SPU Dummy Read
		printf(YELLOW "[MEM] read: Dummy SPU memory read! Ignoring...\n" NORMAL);
	}
	else if ((0x1F800400 <= m_address) && (m_address < 0x1F801040))
	{
		m_return = m_memory_read_dword(m_address & 0xF, m_simplestation->m_memory->m_mem_memctl1);
	}
	else if ((0x1FC00000 <= m_address) && (m_address < 0x1FC80000))
	{
		m_return = m_memory_read_dword(m_address & 0x7FFFF, m_simplestation->m_memory->m_mem_bios);
	}
	else if (m_address < 0x00200000)
	{
		m_return = m_memory_read_dword(m_address & 0x1FFFFF, m_simplestation->m_memory->m_mem_ram);
	}
	else if (m_address == 0xFFFE0130)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] read: Cache Control Register (Current Value: 0x%X)\n" NORMAL, m_simplestation->m_memory->m_memory_cache_control_reg);
#endif
		m_return = m_simplestation->m_memory->m_memory_cache_control_reg;
	}
	else
	{
		printf(RED "[MEM] read: Region not implemented!\n" NORMAL);
		printf("Address: 0x%08X; Offset: 0x%08X\n", m_address, m_memory_offset);
		return m_simplestation_exit(m_simplestation, 1);
	}

 	return m_return;
}

/*
	Function:
	m_memory_write(uint32_t m_memory_offset, uint32_t m_value)

	Arguments:
	-> m_memory_offset: Offset where to look the value at
	-> m_value: Value to be written

	Description:
	Returns a double-word (32-bit value) located at that memory offset
*/
uint32_t m_memory_write(uint32_t m_memory_offset, uint32_t m_value, m_simplestation_state *m_simplestation)
{
	uint32_t m_placeholder, m_address, m_return = 0;

	// PSX doesn't permit unaligned memory writes
	if (m_memory_offset % 4 != 0)
	{
		printf(RED "[MEM] write: Unaligned memory write! Exiting...\n" NORMAL);
		return m_simplestation_exit(m_simplestation, 1);
	}

	// Calculate the absolute memory address to write at
	m_placeholder = m_memory_offset >> 29;
	m_address = m_memory_offset & m_memory_map[m_placeholder];

	if (m_address == 0x1F801060)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] write: RAM_SIZE Register (Current Value: 0x%X, New Value: 0x%X)\n" NORMAL, m_simplestation->m_memory->m_memory_cache_control_reg, m_value);
#endif
		m_simplestation->m_memory->m_memory_ram_config_reg = m_value;
		m_return = m_simplestation->m_memory->m_memory_ram_config_reg;
	}
	else if ((0x1F800000 <= m_address) && (m_address < 0x1F800400))
	{
		m_return = m_memory_write_dword(m_address & 0x3FF, m_value, m_simplestation->m_memory->m_mem_scratchpad);
	}
	else if ((0x1F800400 <= m_address) && (m_address < 0x1F801040))
	{
		m_return = m_memory_write_dword(m_address & 0xF, m_value, m_simplestation->m_memory->m_mem_memctl1);
	}
	else if (m_address < 0x1F802000)
	{
		// SPU Dummy Write
		printf(YELLOW "[MEM] write: Dummy SPU memory write! Ignoring...\n" NORMAL);
	}
	else if ((0x1FC00000 <= m_address) && (m_address < 0x1FC80000))
	{
		m_return = m_interrupts_write(m_address, m_value, m_simplestation);
	}
	else if (m_address < 0x00200000)
	{
		m_return = m_memory_write_dword(m_address & 0x1FFFFF, m_value, m_simplestation->m_memory->m_mem_ram);
	}
	else if (m_address == 0xFFFE0130)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] write: Cache Control Register (Current Value: 0x%X, New Value: 0x%X)\n" NORMAL, m_simplestation->m_memory->m_memory_cache_control_reg , m_value);
#endif
		m_simplestation->m_memory->m_memory_cache_control_reg = m_value;
		m_return = m_simplestation->m_memory->m_memory_cache_control_reg;
	}
	else
	{
		printf(RED "[MEM] write: Region not implemented!\n" NORMAL);
		printf("Address: 0x%08X; Offset: 0x%08X\n", m_address, m_memory_offset);
		return m_simplestation_exit(m_simplestation, 1);
	}

	return m_return;
}