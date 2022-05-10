#ifndef SIMPLESTATION_H
#define SIMPLESTATION_H

#include <cpu/bios.h>
#include <stdint.h>
#include <string.h>

#ifdef NON_C23
#include <stdbool.h>
#endif

/* Defines */
#define KiB 1024
#define MiB (KiB * KiB)

#define OFF false
#define ON true

/* Structures */
typedef struct
{
	bool m_memory_state;
	bool m_interrupts_state;
	bool m_cpu_state;
} m_simplestation_state;

extern m_simplestation_state m_simplestation;

/* Functions */
uint8_t m_simplestation_exit(m_simplestation_state *m_simplestation, uint8_t m_is_fatal);

#endif /* SIMPLESTATION_H */