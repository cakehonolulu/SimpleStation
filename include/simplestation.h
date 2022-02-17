#ifndef SIMPLESTATION_H
#define SIMPLESTATION_H

#include <cpu/cpu.h>
#include <cpu/bios.h>
#include <memory/memory.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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
uint8_t m_simplestation_exit();

#endif /* SIMPLESTATION_H */