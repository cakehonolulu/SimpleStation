#ifndef SIMPLESTATION_H
#define SIMPLESTATION_H

#include <stdint.h>
#include <string.h>

#ifdef PREC23
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

// Different types of memory granularity
typedef enum {byte, word, dword} m_memory_size;

// Delay Slot Helper Structure
typedef struct m_mips_r3000a_delay_slot {
   uint8_t m_register;
   uint32_t m_value;
   m_memory_size m_size;
} m_mips_r3000a_delay_slot_t;

// Struct containing main CPU state
typedef struct m_corewave_cw33300
{
	// Program Counter Register
	uint32_t m_pc;

	// Following Program Counter (abs(4) byte-offsetted)
	uint32_t m_pc_nxt;

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

    m_mips_r3000a_delay_slot_t m_cpu_delayed_memory_load;

	bool m_branch;
	uint32_t m_pre_ds_pc;

} m_mips_r3000a_t;

#define M_R3000_COP0_REGISTERS 32

typedef struct m_corewave_cw33300_cop0
{
	uint32_t m_registers[M_R3000_COP0_REGISTERS];

} m_mips_r3000a_cop0_t;

typedef struct m_cpu_ints
{	
	uint32_t m_interrupt_stat;
	uint32_t m_interrupt_mask;
} m_psx_cpu_ints_t;

typedef enum {
	to_ram = 0,
	from_ram = 1
} m_dma_directions;

typedef enum {
	increment = 0,
	decrement = 1
} m_dma_step;

typedef enum {
	manual = 0,
	request = 1,
	linked_list = 2
} m_dma_sync;

typedef enum {
	mdec_in = 0,
	mdec_out = 1,
	gpu = 2,
	cdrom = 3,
	spu = 4,
	pio = 5,
	otc = 6
} m_dma_ports;

typedef struct m_mem_dma_channel
{
	bool m_enabled;
	m_dma_directions m_direction;
	m_dma_step m_step;
	m_dma_sync m_sync;
	bool m_trigger;
	bool m_chop;
	uint8_t m_chop_dma_size;
	uint8_t m_chop_cpu_size;
	uint8_t m_dummy;

	uint32_t m_base;
} m_psx_dma_channel_t;

typedef struct m_mem_dma
{	
	uint32_t m_control_reg;

	bool m_irq_enable;

	uint8_t m_irq_channel_enable;

	uint8_t m_irq_channel_flags;

	bool m_irq_force;
	
	uint8_t m_irq_dummy;

	m_psx_dma_channel_t m_dma_channels[7];
} m_psx_dma_t;

typedef struct m_psx_memory
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

	m_psx_dma_t *m_dma;

} m_psx_memory_t;

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
	bool m_dma_state;
	bool m_interrupts_state;
	bool m_cpu_state;

	uint32_t m_breakpoint;
	uint32_t m_wp;

	bool m_debugger;

	bool m_cond;

} m_simplestation_state;

// Exception types
typedef enum {
	load_error = 0x4,
	store_error = 0x5,
	syscall = 0x8,
	breakpoint = 0x9,
	illegal = 0xA,
	coprocessor = 0xB,
	overflow = 0xC
} m_exc_types;

/* Functions */
uint8_t m_simplestation_exit(m_simplestation_state *m_simplestation, uint8_t m_is_fatal);
extern void m_printregs(m_simplestation_state *m_simplestation);

#endif /* SIMPLESTATION_H */
