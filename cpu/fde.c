#include <cpu/fde.h>

// Simple function that groups the 3 stages of processing
void m_psx_fde()
{
	m_cpu_fetch();
	m_cpu_decode();
	m_cpu_execute();
}