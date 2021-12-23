#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <cpu/bios.h>

/* Function definitions */
uint32_t m_memory_read_dword(uint32_t m_memory_offset, uint8_t *m_memory_source);

/* Macros */
#define READ32_BIOS(m_addr) m_memory_read_dword(m_addr - 0xbfc00000, m_bios_buffer)

#endif /* MEMORY_H */