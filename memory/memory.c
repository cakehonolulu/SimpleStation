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
	m_simplestation->m_memory = (m_psx_memory_t *) calloc(sizeof(m_psx_memory_t), sizeof(m_psx_memory_t));

	if (m_simplestation->m_memory)
	{
		// 2 MiB Total
		m_simplestation->m_memory->m_mem_ram = (int8_t *) calloc((size_t) PSX_MEM, sizeof(int8_t));

		if (m_simplestation->m_memory->m_mem_ram)
		{
			// 1 KiB Total
			m_simplestation->m_memory->m_mem_scratchpad = (int8_t *) calloc((size_t) (1 * KiB), sizeof(int8_t));

			if (m_simplestation->m_memory->m_mem_scratchpad)
			{
				// 0x20 Total (0x1F801020 - 0x1F801000)
				m_simplestation->m_memory->m_mem_memctl1 = (int8_t *) calloc(0x20, sizeof(int8_t));

				if (m_simplestation->m_memory->m_mem_memctl1)
				{
					m_simplestation->m_memory->m_memory_cache_control_reg = 0;
					m_simplestation->m_memory->m_memory_ram_config_reg = 0;
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

uint8_t m_memory_read_byte(uint32_t m_memory_address, int8_t *m_memory_source)
{
	return *(uint8_t*)(m_memory_source + (m_memory_address + 0));
}

uint8_t m_memory_write_byte(uint32_t m_memory_address, uint8_t m_value, int8_t *m_memory_source)
{
	return *(uint8_t *) (m_memory_address + m_memory_source) = m_value;
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
	m_memory_read(uint32_t m_memory_offset, m_memory_size m_size)

	Arguments:
	-> m_memory_offset: Offset where to look the value at
	-> m_size: Size range from 0-2 for memory-block read size ([1-2-4 bytes])

	Description:
	Returns a double-word (32-bit value) located at that memory offset
*/
uint32_t m_memory_read(uint32_t m_memory_offset, m_memory_size m_size, m_simplestation_state *m_simplestation)
{
	uint32_t m_placeholder, m_address, m_return = 0;

	// PSX doesn't permit unaligned memory writes
	switch (m_size)
	{
		case byte:
			break;

		case word:
			if (m_memory_offset % 2 != 0)
			{
				printf(RED "[MEM] write: Unaligned word memory write! Exiting...\n" NORMAL);
				m_return = m_simplestation_exit(m_simplestation, 1);
			}
			break;

		case dword:
			if (m_memory_offset % 4 != 0)
			{
				printf(RED "[MEM] write: Unaligned dword memory write! Exiting...\n" NORMAL);
				m_return = m_simplestation_exit(m_simplestation, 1);
			}
			break;

		default:
			printf(RED "[MEM] write: Unknown Size! (%d)\n" NORMAL, m_size);
			m_return = m_simplestation_exit(m_simplestation, 1);
			break;
	}

	// Calculate the absolute memory address to read at
	m_placeholder = m_memory_offset >> 29;
	m_address = m_memory_offset & m_memory_map[m_placeholder];

	// PSX RAM
	if (m_address < 0x00200000)
	{
		switch (m_size)
		{
			case byte:
				m_return = m_memory_read_byte(m_address & 0x1FFFFF, m_simplestation->m_memory->m_mem_ram);
				break;

			case word:
				break;

			case dword:
				m_return = m_memory_read_dword(m_address & 0x1FFFFF, m_simplestation->m_memory->m_mem_ram);
				break;

			default:
				__builtin_unreachable();
		}
	}
	// PSX Expansion 1
	else if ((0x1F000000 <= m_address) && (m_address < 0x1F400000))
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] read: Detected 'Expansion 1' memory read! No Expansion Pack detected, ignoring...\n" NORMAL);
#endif
		m_return = 0xFF;
	}
	// PSX Scratchpad
	else if ((0x1F800000 <= m_address) && (m_address < 0x1F800400))
	{
		switch (m_size)
		{
			case byte:
				m_return = m_memory_read_byte(m_address & 0x3FF, m_simplestation->m_memory->m_mem_scratchpad);
				break;

			case word:
				break;

			case dword:
				m_return = m_memory_read_dword(m_address & 0x3FF, m_simplestation->m_memory->m_mem_scratchpad);
				break;

			default:
				__builtin_unreachable();
		}
	}
	// PSX Memory Control
	else if ((0x1F800400 <= m_address) && (m_address < 0x1F801040))
	{
		switch (m_size)
		{
			case byte:
				m_return = m_memory_read_byte(m_address & 0xF, m_simplestation->m_memory->m_mem_memctl1);
				break;

			case word:
				break;

			case dword:
				m_return = m_memory_read_dword(m_address & 0xF, m_simplestation->m_memory->m_mem_memctl1);
				break;

			default:
				__builtin_unreachable();
		}
	}
	// PSX RAM Register
	else if (m_address == 0x1F801060)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] read: RAM_SIZE Register (Current Value: 0x%0X)\n" NORMAL, m_simplestation->m_memory->m_memory_ram_config_reg);
#endif
		m_return = m_simplestation->m_memory->m_memory_ram_config_reg;
	}
	// PSX Interrupt Stat Register
	else if (m_address == 0x1F801074)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] read: Interrupt Mask Register (Current Value: 0x%0X)\n" NORMAL, m_simplestation->m_cpu_ints->m_interrupt_mask);
#endif
		m_return = m_simplestation->m_cpu_ints->m_interrupt_mask;
	}
	// PSX SPU
	else if (m_address < 0x1F802000)
	{
		// SPU Dummy Read
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] read: Dummy SPU memory read! Ignoring...\n" NORMAL);
#endif
	}
	// PSX Expansion 2
	else if ((0x1F802000 <= m_address) && (m_address < 0x1F804000))
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] read: Detected 'Expansion 2' memory read! Ignoring...\n" NORMAL);
#endif	
	}
	// PSX BIOS Space
	else if ((0x1FC00000 <= m_address) && (m_address < 0x1FC80000))
	{
		switch (m_size)
		{
			case byte:
				m_return = m_memory_read_byte(m_address & 0x7FFFF, m_simplestation->m_memory->m_mem_bios);
				break;

			case word:
				break;

			case dword:
				m_return = m_memory_read_dword(m_address & 0x7FFFF, m_simplestation->m_memory->m_mem_bios);
				break;

			default:
				__builtin_unreachable();
		}
	}
	// PSX Cache Control Register
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
		m_return = m_simplestation_exit(m_simplestation, 1);
	}

 	return m_return;
}

/*
	Function:
	m_memory_write(uint32_t m_memory_offset, uint32_t m_value, m_memory_size m_size)

	Arguments:
	-> m_memory_offset: Offset where to look the value at
	-> m_value: Value to be written
	-> m_size: Size range from 0-2 for memory-block read size ([1-2-4 bytes])

	Description:
	Returns a double-word (32-bit value) located at that memory offset
*/
uint32_t m_memory_write(uint32_t m_memory_offset, uint32_t m_value, m_memory_size m_size, m_simplestation_state *m_simplestation)
{
	uint32_t m_placeholder, m_address, m_return = 0;

	// PSX doesn't permit unaligned memory writes
	switch (m_size)
	{
		case byte:
			break;

		case word:
			if (m_memory_offset % 2 != 0)
			{
				printf(RED "[MEM] write: Unaligned word memory write! Exiting...\n" NORMAL);
				m_return = m_simplestation_exit(m_simplestation, 1);
			}
			break;

		case dword:
			if (m_memory_offset % 4 != 0)
			{
				printf(RED "[MEM] write: Unaligned dword memory write! Exiting...\n" NORMAL);
				m_return = m_simplestation_exit(m_simplestation, 1);
			}
			break;

		default:
			printf(RED "[MEM] write: Unknown Size! (%d)\n" NORMAL, m_size);
			m_return = m_simplestation_exit(m_simplestation, 1);
			break;
	}

	// Calculate the absolute memory address to write at
	m_placeholder = m_memory_offset >> 29;
	m_address = m_memory_offset & m_memory_map[m_placeholder];

	// PSX RAM
	if (m_address < 0x00200000)
	{
		switch (m_size)
		{
			case byte:
				m_return = m_memory_write_byte(m_address & 0x1FFFFF, m_value, m_simplestation->m_memory->m_mem_ram);
				break;

			case word:
				break;

			case dword:
				m_return = m_memory_write_dword(m_address & 0x1FFFFF, m_value, m_simplestation->m_memory->m_mem_ram);
				break;

			default:
				__builtin_unreachable();
		}
	}
	// PSX Expansion 1
	else if ((0x1F000000 <= m_address) && (m_address < 0x1F400000))
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] write: Detected 'Expansion 1' memory write! Ignoring...\n" NORMAL);
#endif
	}
	// PSX Scratchpad
	else if ((0x1F800000 <= m_address) && (m_address < 0x1F800400))
	{
		switch (m_size)
		{
			case byte:
				m_return = m_memory_write_byte(m_address & 0x3FF, m_value, m_simplestation->m_memory->m_mem_scratchpad);
				break;

			case word:
				break;

			case dword:
				m_return = m_memory_write_dword(m_address & 0x3FF, m_value, m_simplestation->m_memory->m_mem_scratchpad);
				break;

			default:
				__builtin_unreachable();
		}
	}
	// PSX Memory Control
	else if ((0x1F800400 <= m_address) && (m_address < 0x1F801040))
	{
		switch (m_size)
		{
			case byte:
				m_return = m_memory_write_byte(m_address & 0xF, m_value, m_simplestation->m_memory->m_mem_memctl1);
				break;

			case word:
				break;

			case dword:
				m_return = m_memory_write_dword(m_address & 0xF, m_value, m_simplestation->m_memory->m_mem_memctl1);
				break;

			default:
				__builtin_unreachable();
		}
	}
	// PSX's RAM Register
	else if (m_address == 0x1F801060)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] write: RAM_SIZE Register (Current Value: 0x%X, New Value: 0x%X)\n" NORMAL, m_simplestation->m_memory->m_memory_cache_control_reg, m_value);
#endif
		m_simplestation->m_memory->m_memory_ram_config_reg = m_value;
		m_return = m_simplestation->m_memory->m_memory_ram_config_reg;
	}
	// PSX Interrupt Stat Register
	else if (m_address == 0x1F801074)
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] write: Interrupt Mask Register (Current Value: 0x%0X, New Value: 0x%X)\n" NORMAL, m_simplestation->m_cpu_ints->m_interrupt_mask, m_value);
#endif
		m_simplestation->m_cpu_ints->m_interrupt_mask = m_value;
		m_return = m_simplestation->m_cpu_ints->m_interrupt_mask;
	}
	// PSX SPU
	else if (m_address < 0x1F802000)
	{
		// SPU Dummy Write
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] write: Dummy SPU memory write! Ignoring...\n" NORMAL);
#endif
	}
	// PSX Expansion 2
	else if ((0x1F802000 <= m_address) && (m_address < 0x1F804000))
	{
#ifdef DEBUG_MEMORY
		printf(YELLOW "[MEM] write: Detected 'Expansion 2' memory write! Ignoring...\n" NORMAL);
#endif
	}
	// PSX Interrupts
	else if ((0x1FC00000 <= m_address) && (m_address < 0x1FC80000))
	{
		m_return = m_interrupts_write(m_address, m_value, m_simplestation);
	}
	// PSX Cache Control Register
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
		m_return = m_simplestation_exit(m_simplestation, 1);
	}

	return m_return;
}