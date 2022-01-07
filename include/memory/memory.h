#ifndef MEMORY_H
#define MEMORY_H

#include <cpu/bios.h>
#include <cpu/interrupts.h>
#include <simplestation.h>
#include <stdint.h>

// Main EDO RAM Size on Sony's PSX (2 MiB) 
#define PSX_MEM (2 * MiB)

// BIOS Size on Sony's PSX (512 KiB) 
#define PSX_MEM_BIOS (MiB / 2)

typedef enum {byte, word, dword} m_memory_size;

/* Function definitions */
uint32_t m_memory_read(uint32_t m_memory_offset, m_memory_size m_type);
uint32_t m_memory_read_dword(uint32_t m_memory_address, int8_t *m_memory_source);
void m_memory_init();
void m_memory_exit();

extern int8_t *m_mem_ram;
extern int8_t *m_mem_bios;

/* Macros */
#define READ32_BIOS(m_addr) m_memory_read(m_addr, dword)

#endif /* MEMORY_H */