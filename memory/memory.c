#include <memory/memory.h>

/*
	m_memory_read_dword

	Reads a double word at memory location specified by the offset

	It then crafts an endianes--aware 32-bit representation of it using carefully
	crafted bitshifts; this is due to the little-endianess of the PSX.
*/
uint32_t m_memory_read_dword(uint32_t m_memory_offset, uint8_t *m_memory_source)
{
	uint8_t m_byte0 = (uint8_t) m_memory_source[m_memory_offset + 0];
	uint8_t m_byte1 = (uint8_t) m_memory_source[m_memory_offset + 1];
	uint8_t m_byte2 = (uint8_t) m_memory_source[m_memory_offset + 2];
	uint8_t m_byte3 = (uint8_t) m_memory_source[m_memory_offset + 3];

	return (uint32_t) (m_byte0 | (m_byte1 << 8) | (m_byte2 << 16) | (m_byte3 << 24));
}