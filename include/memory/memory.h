#ifndef MEMORY_H
#define MEMORY_H

#include <cpu/bios.h>
#include <simplestation.h>
#include <stdint.h>

// Main EDO RAM Size on Sony's PSX (2 MiB) 
#define PSX_MEM (2 * MiB)

// BIOS Size on Sony's PSX (512 KiB) 
#define PSX_MEM_BIOS (MiB / 2)

/* Function definitions */
uint32_t m_memory_read(uint32_t m_memory_address, int8_t *m_memory_source);
uint32_t m_memory_read_dword(uint32_t m_memory_offset);
void m_memory_init();
void m_memory_exit();

extern int8_t *m_mem_ram;
extern int8_t *m_mem_bios;

/* Macros */
#define READ32_BIOS(m_addr) m_memory_read_dword(m_addr)

#endif /* MEMORY_H */