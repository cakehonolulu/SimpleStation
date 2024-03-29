#include <memory/memory.h>
#include <cpu/instructions.h>
#include <cpu/interrupts.h>
#include <cpu/bios.h>
#include <gpu/gpu.h>
#include <cdrom/cdrom.h>
#include <spu/spu.h>
#include <ui/termcolour.h>
#if defined (__unix__)
#include <endian.h>
#endif
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

					if (m_dma_init(m_simplestation) == 0)
					{
						m_simplestation->m_memory_state = ON;
					}
					else
					{
						printf("[MEM] init: Couldn't initialize PSX's DMA, exiting...");
						m_return = 1;
					}
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

	m_exc_types m_exc = load_error;

	// PSX doesn't permit unaligned memory writes
	switch (m_size)
	{
		case byte:
			break;

		case word:
			if (m_memory_offset % 2 != 0)
			{
				m_exception(m_exc, m_simplestation);
			}
			break;

		case dword:
			if (m_memory_offset % 4 != 0)
			{
				m_exception(m_exc, m_simplestation);
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
	switch (m_address)
	{
		case 0x00000000 ... 0x001FFFFF:
			m_return = m_memory_read_handle(m_address & 0x1FFFFF, m_simplestation->m_memory->m_mem_ram, m_size, m_simplestation);
			break;

		case 0x1F000000 ... 0x1F3FFFFF:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: Detected 'Expansion 1' memory read! No Expansion Pack detected, ignoring...\n" NORMAL);
#endif
			m_return = 0xFF;
			break;

		case 0x1F800000 ... 0x1F8003FF:
			m_return = m_memory_read_handle(m_address & 0x3FF, m_simplestation->m_memory->m_mem_scratchpad, m_size, m_simplestation);
			break;
		
		case 0x1F800400 ... 0x1F80103F:
			m_return = m_memory_read_handle(m_address & 0xF, m_simplestation->m_memory->m_mem_memctl1, m_size, m_simplestation);
			break;

		case 0x1F801040:
			// TODO: Properly handle this, don't hardcode
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: JOY_RX_DATA read, ignoring...\n" NORMAL);
#endif
			m_return = 0xFF;
			break;

		case 0x1F801044:
			// TODO: Properly handle this, don't hardcode
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: JOY_STAT read, ignoring...\n" NORMAL);
#endif
			m_return = 0x7;
			break;

		case 0x1F80104A:
			// TODO: Properly handle this, don't hardcode
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: JOY_CTRL read, ignoring...\n" NORMAL);
#endif
			m_return = 0x0;
			break;

		case 0x1F801060:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: RAM_SIZE Register (Current Value: 0x%0X)\n" NORMAL, m_simplestation->m_memory->m_memory_ram_config_reg);
#endif
			m_return = m_simplestation->m_memory->m_memory_ram_config_reg;
			break;

		case 0x1F801070 ... 0x1F801078:
			m_return = m_interrupts_read(m_address & 0xF, m_simplestation);
			break;
		
		case 0x1F801080 ... 0x1F8010FF:
			// DMA Registers Read
			m_return = m_dma_read((m_address - 0x1F801080), m_simplestation);
			break;

		case 0x1F801100 ... 0x1F801130:
			// Timer Registers Dummy Read
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: Dummy Timer Registers memory read! Ignoring...\n" NORMAL);
#endif
			if (m_address == 0x1F801120)
			{
				m_return = 0x000016B0;
			}
			else
			{
				m_return = 0;
			}
			break;

		case 0x1F801810 ... 0x1f801817:
			// PSX GPU Dummy Read
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: GPU Registers read!\n" NORMAL);
#endif
			switch(m_address & 0x0000000F)
			{
				case 0:
#ifdef DEBUG_MEMORY
					printf(CYAN "[MEM] read: GPUREAD Read..\n" NORMAL);
#endif
					m_return = m_gpu_get_read(m_simplestation);
					break;

				case 4:
#ifdef DEBUG_MEMORY
					printf(CYAN "[MEM] read: GPUSTAT Read..\n" NORMAL);
#endif
					m_return = m_gpu_get_status(m_simplestation);
					break;

				default:
					printf(RED "[MEM] read: Unhandled GPU Read (Offset 0x%02X)!\n" NORMAL, (m_address & 0x0000000F));
					m_simplestation_exit(m_simplestation, 1);
					break;
			}

			break;

		// CDROM Read
		case 0x1F801800 ... 0x1F801803:
			m_return = m_cdrom_read(m_address & 0x0000000F, m_simplestation);
			break;

		case 0x1F801C00 ... 0x1F801FFF:
			// SPU Dummy Read
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: Dummy SPU memory read! Ignoring...\n" NORMAL);
#endif
			m_return = m_spu_read(m_address, m_simplestation);
			break;
		
		case 0x1F802000 ... 0x1F803FFF:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: Detected 'Expansion 2' memory read! Ignoring...\n" NORMAL);
#endif
			break;

		case 0x1FC00000 ... 0x1FC7FFFF:
			m_return = m_memory_read_handle(m_address & 0x7FFFF, m_simplestation->m_memory->m_mem_bios, m_size, m_simplestation);
			break;

		case 0xFFFE0130:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] read: Cache Control Register (Current Value: 0x%X)\n" NORMAL, m_simplestation->m_memory->m_memory_cache_control_reg);
#endif
			m_return = m_simplestation->m_memory->m_memory_cache_control_reg;
			break;

		default:
			printf(RED "[MEM] read: Region not implemented!\n" NORMAL);
			printf("Address: 0x%08X; Offset: 0x%08X\n", m_address, m_memory_offset);
			m_return = m_simplestation_exit(m_simplestation, 1);
			break;
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

	m_exc_types m_exc = store_error;

	// PSX doesn't permit unaligned memory writes
	switch (m_size)
	{
		case byte:
			break;

		case word:
			if (m_memory_offset % 2 != 0)
			{
				m_exception(m_exc, m_simplestation);
			}
			break;

		case dword:
			if (m_memory_offset % 4 != 0)
			{
				m_exception(m_exc, m_simplestation);
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

	switch (m_address)
	{
		case 0x00000000 ... 0x001FFFFF:
			m_memory_write_handle(m_address & 0x1FFFFF, m_value, m_simplestation->m_memory->m_mem_ram, m_size, m_simplestation);
			break;

		case 0x1F000000 ... 0x1F3FFFFF:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: Detected 'Expansion 1' memory write! Ignoring...\n" NORMAL);
#endif
			break;

		case 0x1F800000 ... 0x1F8003FF:
			m_memory_write_handle(m_address & 0x3FF, m_value, m_simplestation->m_memory->m_mem_scratchpad, m_size, m_simplestation);
			break;

		case 0x1F800400 ... 0x1F80103F:
			m_memory_write_handle(m_address & 0xF, m_value, m_simplestation->m_memory->m_mem_memctl1, m_size, m_simplestation);
			break;

		case 0x1F801040:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: JOY_RX_DATA write (Value: 0x%08X), ignoring...\n" NORMAL, m_value);
#endif
			break;

		case 0x1F801048:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: JOY_MODE write (Value: 0x%08X), ignoring...\n" NORMAL, m_value);
#endif
			break;

		case 0x1F80104A:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: JOY_CTRL write (Value: 0x%08X), ignoring...\n" NORMAL, m_value);
#endif
			break;

		case 0x1F80104E:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: JOY_BAUD write (Value: 0x%08X), ignoring...\n" NORMAL, m_value);
#endif
			break;
			
		case 0x1F801060:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: RAM_SIZE Register (Current Value: 0x%X, New Value: 0x%X)\n" NORMAL, m_simplestation->m_memory->m_memory_cache_control_reg, m_value);
#endif
			m_simplestation->m_memory->m_memory_ram_config_reg = m_value;
			m_return = m_simplestation->m_memory->m_memory_ram_config_reg;
			break;

		case 0x1F801070 ... 0x1F801078:
			m_interrupts_write(m_address & 0xF, m_value, m_simplestation);
			break;

		case 0x1F801080 ... 0x1F8010FF:
			// DMA Registers Write
			m_dma_write((m_address - 0x1F801080), m_value, m_simplestation);
			break;

		case 0x1F801100 ... 0x1F80112F:
			// Timer Registers Dummy Write
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: Dummy Timer Registers memory write! Ignoring...\n" NORMAL);
#endif
			break;

		// CDROM Write
		case 0x1F801800 ... 0x1F801803:
			m_cdrom_write(m_address & 0x0000000F, m_value, m_simplestation);
			break;

		case 0x1F801C00 ... 0x1F801FFF:
			// SPU Dummy Write
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: Dummy SPU memory write! Ignoring...\n" NORMAL);
#endif
			m_spu_write(m_address, m_value, m_simplestation);

			break;

		case 0x1F801810 ... 0x1F801817:
			switch (m_address & 0x0000000F)
			{
				case 0:
					m_gpu_gp0(m_value, m_simplestation);
					break;

				case 4:
					m_gpu_gp1(m_value, m_simplestation);
					break;

				default:
					printf(RED "[GPU] write: Unhandled GPU Write! (Offset 0x%02X)\n" NORMAL, (m_address & 0x0000000F));
					m_simplestation_exit(m_simplestation, 1);
					break;
			}
		break;

		case 0x1F802000 ... 0x1F803FFF:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: Detected 'Expansion 2' memory write! Ignoring...\n" NORMAL);
#endif
			break;

		case 0xFFFE0130:
#ifdef DEBUG_MEMORY
			printf(YELLOW "[MEM] write: Cache Control Register (Current Value: 0x%X, New Value: 0x%X)\n" NORMAL, m_simplestation->m_memory->m_memory_cache_control_reg , m_value);
#endif
			m_simplestation->m_memory->m_memory_cache_control_reg = m_value;
			m_return = m_simplestation->m_memory->m_memory_cache_control_reg;
			break;

		default:
			printf(RED "[MEM] write: Region not implemented!\n" NORMAL);
			printf("Address: 0x%08X; Offset: 0x%08X\n", m_address, m_memory_offset);
			m_return = m_simplestation_exit(m_simplestation, 1);
			break;
	}

	return m_return;
}

uint32_t m_memory_read_handle(uint32_t m_memory_address, int8_t *m_memory_source, m_memory_size m_size, m_simplestation_state *m_simplestation)
{
	uint32_t m_value = 0;

	switch (m_size)
	{
		case byte:
			m_value = m_memory_read_byte(m_memory_address, m_memory_source);
			break;

		case word:
			m_value = m_memory_read_word(m_memory_address, m_memory_source);
			break;

		case dword:
			m_value = m_memory_read_dword(m_memory_address, m_memory_source);
			break;
		
		default:
			printf(RED "[MEM] read: Unknown Size! (%d)\n" NORMAL, m_size);
			m_value = m_simplestation_exit(m_simplestation, 1);
			break;
	}

	return m_value;
}

void m_memory_write_handle(uint32_t m_memory_address, uint32_t m_value, int8_t *m_memory_source, m_memory_size m_size, m_simplestation_state *m_simplestation)
{
	switch (m_size)
	{
		case byte:
			m_memory_write_byte(m_memory_address, m_value, m_memory_source);
			break;

		case word:
			m_memory_write_word(m_memory_address, m_value, m_memory_source);
			break;

		case dword:
			m_memory_write_dword(m_memory_address, m_value, m_memory_source);
			break;
		
		default:
			printf(RED "[MEM] write: Unknown Size! (%d)\n" NORMAL, m_size);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}


uint8_t m_memory_read_byte(uint32_t m_memory_address, int8_t *m_memory_source)
{
	return *(uint8_t*)(m_memory_source + (m_memory_address + 0));
}

uint8_t m_memory_write_byte(uint32_t m_memory_address, uint8_t m_value, int8_t *m_memory_source)
{
	return *(uint8_t *) (m_memory_address + m_memory_source) = m_value;
}

uint16_t m_memory_read_word(uint32_t m_memory_address, int8_t *m_memory_source)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	/*
		If on LE, it's safe to assume we'll read the bytes in the order we want them.
		This reduces the function size by a large margin, and therefor, performs faster.
	*/
	return *(uint16_t*)(m_memory_source + (m_memory_address + 0));
#else
	/*
		Else, if on BE, use the legacy fallback method; it's clunkier and slower but works.
	*/
	uint8_t b0 = *(uint8_t*)(m_memory_source + (m_memory_address + 0));
	uint8_t b1 = *(uint8_t*)(m_memory_source + (m_memory_address + 1));

	return (b0 | (b1 << 8));
#endif
}

uint16_t m_memory_write_word(uint32_t m_memory_address, uint16_t m_value, int8_t *m_memory_source)
{
	return *(uint16_t *) (m_memory_address + m_memory_source) = m_value;
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

uint32_t utils_LoadLittleEndianInt(uint8_t *data, uint32_t offset) {
	uint8_t b0, b1, b2, b3;
	// Convert to little endian
	b0 = data[offset + 0];
	b1 = data[offset + 1];
	b2 = data[offset + 2];
	b3 = data[offset + 3];
	return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

void* loadFile( const char* filename ) {
	FILE* f = fopen( filename, "rb" );
	fseek( f, 0, SEEK_END );
	size_t fsize = ftell( f );
	rewind( f );
	void* data = malloc( fsize );
	fread( data, fsize, 1, f );
	return data;
}
