#ifndef SIMPLESTATION_H
#define SIMPLESTATION_H

#include <stdint.h>
#include <string.h>

#if !(__STDC_VERSION__ > 201710L)
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
	uint16_t m_block_size;
	uint16_t m_block_count;
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


typedef enum
{
    t4bit = 0,
    t8bit = 1,
    t15bit = 2
} texture_depth;

typedef enum
{
    top = 1,
    bottom = 0
} field;

typedef enum
{
    y240lines = 0,
    y480lines = 1
} vertical_res;

typedef enum
{
    ntsc = 0,
    pal = 1
} video_mode;

typedef enum
{
    d15bits = 0,
    d24bits = 1
} display_depth;

typedef enum
{
    off = 0,
    fifo = 1,
    cpu_to_gp0 = 2,
    vram_to_cpu = 3
} dma_direction;

typedef enum
{
	command,
	image_load
} gp0_mode;

typedef struct
{
	uint32_t m_buffer[12];
	uint8_t m_length;
} m_psx_gpu_command_buffer_t;

#define MAX_IMAGE_SIZE 64 *  256
#define IMAGEBUFFER_MAX 32 * MAX_IMAGE_SIZE

typedef struct {
	uint16_t x, y;
	uint16_t w, h;
	uint32_t index;
	// Buffer used for temp storing of single images
	uint32_t buffer[(1024 * 512)];
	uint32_t image_index;

} m_psx_gpu_image_buffer_t;

typedef struct m_gpu
{
	uint8_t m_page_base_x;
	uint8_t m_page_base_y;

	uint8_t m_semitransparency;
    texture_depth m_texture_depth;
    bool m_dithering;
    bool m_draw_to_display;
    bool m_force_set_mask_bit;
    bool m_preserve_masked_pixels;
    field m_field;
    bool m_texture_disable;
    uint8_t m_horizontal_resolution;
    vertical_res m_vertical_resolution;
    video_mode m_video_mode;
    display_depth m_display_depth;
    bool m_interlaced;
    bool m_display_disabled;
    bool m_interrupt;
    dma_direction m_dma_direction;

	bool m_rectangle_texture_x_flip;
	bool m_rectangle_texture_y_flip;

	uint8_t m_texture_window_x_mask;
	uint8_t m_texture_window_y_mask;
	
	uint8_t m_texture_window_x_offset;
	uint8_t m_texture_window_y_offset;

	uint16_t m_drawing_area_left;
	uint16_t m_drawing_area_top;
	uint16_t m_drawing_area_right;
	uint16_t m_drawing_area_bottom;
	
	int16_t m_drawing_x_offset;
	int16_t m_drawing_y_offset;

	uint16_t m_display_vram_x_start;
	uint16_t m_display_vram_y_start;
	
	uint16_t m_display_horizontal_start;
	uint16_t m_display_horizontal_end;
	uint16_t m_display_line_start;
	uint16_t m_display_line_end;

	uint32_t m_gp0_words_remaining;
	uint32_t m_gp0_current_cmd;

	gp0_mode m_gp0_mode;

	uint32_t m_gp0_cmd_ins;
	uint32_t m_gp0_instruction;

} m_psx_gpu_t;

typedef struct {
	uint32_t m_status;
	uint32_t m_command;
	uint32_t m_fifo;
	uint32_t m_req;

} m_psx_cdrom_t;

typedef struct {
	uint32_t initial_pc;
	uint32_t initial_gp;
	uint32_t ram_destination;
	uint32_t file_size;
	uint32_t fill_start_address;
	uint32_t fill_size;
	uint32_t initial_spfp_base;
	uint32_t initial_spfp_off;
	uint32_t marker;
} ExeFile;

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

	m_psx_gpu_t *m_gpu;

	m_psx_gpu_command_buffer_t *m_gpu_command_buffer;

	m_psx_gpu_image_buffer_t *m_gpu_image_buffer;

	m_psx_cdrom_t *m_cdrom;

	ExeFile *exe;
	uint8_t *exename;
	bool m_sideload;
	bool m_tty;

	bool m_memory_state;
	bool m_dma_state;
	bool m_interrupts_state;
	bool m_cpu_state;
	bool m_gpu_state;
	bool m_gpu_command_buffer_state;
	bool m_cdrom_state;

	uint32_t m_breakpoint;
	uint32_t m_wp;

#ifdef DUMP_VRAM
	uint8_t *m_vram_data;
#endif

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
