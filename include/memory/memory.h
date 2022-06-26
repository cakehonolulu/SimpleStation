#ifndef MEMORY_H
#define MEMORY_H

#include <simplestation.h>
#include <stdint.h>

// Main EDO RAM Size on Sony's PSX (2 MiB) 
#define PSX_MEM (2 * MiB)

// BIOS Size on Sony's PSX (512 KiB) 
#define PSX_MEM_BIOS (MiB / 2)

/* Function definitions */
uint32_t m_memory_read(uint32_t m_memory_offset, m_simplestation_state *m_simplestation);
uint32_t m_memory_write(uint32_t m_memory_offset, uint32_t m_value, m_simplestation_state *m_simplestation);
uint32_t m_memory_read_dword(uint32_t m_memory_address, int8_t *m_memory_source);
uint8_t m_memory_init(m_simplestation_state *m_simplestation);
void m_memory_exit(m_simplestation_state *m_simplestation);

/* Macros */
#define READ32_BIOS(m_addr) m_memory_read(m_addr, dword)

#endif /* MEMORY_H */