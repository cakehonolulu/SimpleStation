#ifndef SIMPLESTATION_H
#define SIMPLESTATION_H

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

#define M_R3000_REGISTERS 32

/*
	Sony Playstation 1 (PSX) CPU Header
	
	LSI CoreWare CW33300-based core:
	MIPS R3000A-compatible 32-bit RISC CPU (MIPS R3051)
	5 KB L1 cache @ 33.8688MHz
*/

// Struct containing main CPU state
typedef struct m_corewave_cw33300
{
	// Program Counter Register
	uint32_t m_pc;

	/*
		High 32 bits of multiplication result
		Remainder of division
	*/
	uint32_t m_hi;

	/*
		Low 32 bits of multiplication result
		Quotient of division
	*/
	uint32_t m_lo;

	/*
		32 general prupose registers
	*/
	uint32_t m_registers[M_R3000_REGISTERS];

	// Current opcode
	uint32_t m_opcode;

	uint32_t m_next_opcode;
} m_mips_r3000a_t;

#define M_R3000_COP0_REGISTERS 32

typedef struct m_corewave_cw33300_cop0
{
	uint32_t m_registers[M_R3000_COP0_REGISTERS];

} m_mips_r3000a_cop0_t;

typedef struct m_psx_memmory
{
	// PSX RAM Memory Buffer
	int8_t *m_mem_ram;

	// PSX Scratchpad RAM
	int8_t *m_mem_scratchpad;

	// PSX Memory Control 1 RAM
	int8_t *m_mem_memctl1;

	// PSX BIOS Memory Buffer
	int8_t *m_mem_bios;
	
	// PSX Memory Registers
	uint32_t m_memory_ram_config_reg;
	uint32_t m_memory_cache_control_reg;

} m_psx_memory_t;

typedef struct m_cpu_ints
{	
	uint32_t m_interrupt_stat;
	uint32_t m_interrupt_mask;
} m_psx_cpu_ints_t;

/* Structures */
typedef struct
{
	// Declare a pointer to the CPU state structure
	m_mips_r3000a_t *m_cpu;

	// Declare a pointer to the CPU COP0 state structure
	m_mips_r3000a_cop0_t *m_cpu_cop0;

	// Declare a pointer to the PSX Memory state structure
	m_psx_memory_t *m_memory;

	m_psx_cpu_ints_t *m_cpu_ints;

	bool m_memory_state;
	bool m_interrupts_state;
	bool m_cpu_state;

} m_simplestation_state;

/* Functions */
uint8_t m_simplestation_exit(m_simplestation_state *m_simplestation, uint8_t m_is_fatal);

#endif /* SIMPLESTATION_H */