#ifndef SIMPLESTATION_H
#define SIMPLESTATION_H

#include <stdint.h>
#include <string.h>

#if !(__STDC_VERSION__ > 201710L)
#include <stdbool.h>
#endif

#ifdef GDBSTUB_SUPPORT
#include <extern/gdbstub/gdbstub.h> 
#endif

#include <renderer/vulkan/vulcano.h>

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
	uint32_t m_pc_cur;

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
	bool m_delay;
	uint32_t m_pre_ds_pc;

} m_mips_r3000a_t;

#define M_R3000_COP0_REGISTERS 32

typedef struct m_corewave_cw33300_cop0
{
	uint32_t m_registers[M_R3000_COP0_REGISTERS];

} m_mips_r3000a_cop0_t;

typedef union {
	struct {
		uint8_t l, h, h2, h3;
	} b;

	struct {
		uint16_t l, h;
	} w;

	struct {
		int8_t l, h, h2, h3;
	} sb;

	struct {
		int16_t l, h;
	} sw;

	uint32_t d;
	int32_t sd;
} pair;
	
typedef union {
	struct {
		int16_t vx0, vy0, vz0;
		int16_t vx1, vy1, vz1;
		int16_t vx2, vy2, vz2;
		uint8_t r, g, b, c;
		int32_t otz;
		int32_t ir0, ir1, ir2, ir3;
		int16_t sx0, sy0;
		int16_t sx1, sy1;
		int16_t sx2, sy2;
		int16_t sz0, n0;
		int16_t sz1, n1;
		int16_t sz2, n2;
		uint8_t r0, g0, b0, c0;
		uint8_t r1, g1, b1, c1;
		uint8_t r2, g2, b2, c2;
		int32_t reserved;
		int32_t mac0, mac1, mac2, mac3;
		uint32_t irgb, orgb;
		int32_t lzcs, lzcr;
	} regs;

	uint32_t raw[32];
	pair r[32];
} cop2d_;


typedef union {
	struct {
		int16_t r11, r12, r13, r21, r22, r23, r31, r32, r33, n0;
		int32_t trX, trY, trZ;
		int16_t l11, l12, l13, l21, l22, l23, l31, l32, l33, n1;
		int32_t rbk, gbk, bbk;
		int16_t lr1, lr2, lr3, lg1, lg2, lg3, lb1, lb2, lb3, n2;
		int32_t rfc, gfc, bfc;
		int32_t ofx, ofy;
		int32_t h;
		int32_t dqa, dqb;
		int32_t zsf3, zsf4;
		int32_t flag;
	} regs;
	
	uint32_t raw[32];
	pair r[32];
} cop2c_;

typedef struct m_psx_gte
{
	cop2d_ cop2d;
	cop2c_ cop2c;
} m_psx_gte_t;

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
    video_to_cpu = 3
} dma_direction;

typedef enum
{
	command,
	cpu_to_vram,
	vram_to_cpu
} gp0_mode;

typedef enum
{
	XRes256,
	XRes320,
	XRes512,
	XRes640,
	XRes368
} horizontalRes;

typedef enum
{
	VRes240 = false,
	VRes480 = true
} verticalRes;

typedef struct
{
	uint32_t m_buffer[12];
	uint8_t m_length;
} m_psx_gpu_command_buffer_t;

#define MAX_IMAGE_SIZE 64 *  256
#define IMAGEBUFFER_MAX 32 * MAX_IMAGE_SIZE

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
    horizontalRes m_horizontal_resolution;
    verticalRes m_vertical_resolution;
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

	gp0_mode m_gp0_write_mode;
	gp0_mode m_gp0_read_mode;

	uint32_t m_vram_image_size;
	uint32_t m_vram_image_index;

	uint32_t m_gp0_cmd_ins;
	uint32_t m_gp0_instruction;
	
	// VRAM Buffers for Copy Operations
	uint32_t write_buffer[(1024 * 512)];
	uint32_t read_buffer[(1024 * 512)];

} m_psx_gpu_t;


/*
	1F801800h - Index/Status Register (Bit0-1 R/W) (Bit2-7 Read Only)
	0-1 Index   Port 1F801801h-1F801803h index (0..3 = Index0..Index3)   (R/W)
	2   ADPBUSY XA-ADPCM fifo empty  (0=Empty) ;set when playing XA-ADPCM sound
	3   PRMEMPT Parameter fifo empty (1=Empty) ;triggered before writing 1st byte
	4   PRMWRDY Parameter fifo full  (0=Full)  ;triggered after writing 16 bytes
	5   RSLRRDY Response fifo empty  (0=Empty) ;triggered after reading LAST byte
	6   DRQSTS  Data fifo empty      (0=Empty) ;triggered after reading LAST byte
	7   BUSYSTS Command/parameter transmission busy  (1=Busy)
*/
typedef union {
	struct {
		uint8_t index : 2;
		uint8_t XAADCPMFifoEmpty : 1;
		uint8_t parameterFifoEmpty : 1;
		uint8_t parameterFifoFull : 1;
		uint8_t responseFifoEmpty : 1;
		uint8_t dataFifoEmpty : 1;
		uint8_t transmissionBusy : 1;
	};

   	uint8_t _value;
} CDROMStatus;

/*
	1F801803h.Index0 - Interrupt Enable Register (R)
	1F801803h.Index2 - Interrupt Enable Register (R) (Mirror)
	0-4  Interrupt Enable Bits (usually all set, ie. 1Fh=Enable All IRQs)
	5-7  Unknown/unused (write: should be zero) (read: usually all bits set)
*/
typedef union
	{
		struct {
			uint8_t enable : 5;
       		uint8_t unknown : 3;
		};

   	uint8_t _value;
} CDROMInterrupt;

/*
	1F801803h.Index1 - Interrupt Flag Register (R/W)
	1F801803h.Index3 - Interrupt Flag Register (R) (Mirror)
	0-2   Read: Response Received   Write: 7=Acknowledge   ;INT1..INT7
	3     Read: Unknown (usually 0) Write: 1=Acknowledge   ;INT8  ;XXX CLRBFEMPT
	4     Read: Command Start       Write: 1=Acknowledge   ;INT10h;XXX CLRBFWRDY
	5     Read: Always 1 ;XXX "_"   Write: 1=Unknown              ;XXX SMADPCLR
	6     Read: Always 1 ;XXX "_"   Write: 1=Reset Parameter Fifo ;XXX CLRPRM
	7     Read: Always 1 ;XXX "_"   Write: 1=Unknown              ;XXX CHPRST
*/
typedef union
	{
   		struct {
			uint8_t responseReceived : 3;
			uint8_t unknown : 1;
			uint8_t commandStart : 1;
			uint8_t unknown2 : 1;
			uint8_t unknown3 : 1;
			uint8_t unknown4 : 1;
		};

    uint8_t _value;

} CDROMInterruptFlag;



/*
	Status code (stat)
	0  Error         Invalid Command/parameters (followed by Error Byte)
	1  Spindle Motor (0=Motor off, or in spin-up phase, 1=Motor on)
	2  SeekError     (0=Okay, 1=Seek error)     (followed by Error Byte)
	3  IdError       (0=Okay, 1=GetID denied) (also set when Setmode.Bit4=1)
	4  ShellOpen     Once shell open (0=Closed, 1=Is/was Open)
	5  Read          Reading data sectors  ;/set until after Seek completion)
	6  Seek          Seeking               ; at a time (ie. Read/Play won't get
	7  Play          Playing CD-DA         ;\only ONE of these bits can be set
*/
typedef union {
    struct {
        uint8_t error : 1;
        uint8_t spindleMotor : 1;
        uint8_t seekError : 1;
        uint8_t getIdError : 1;
        uint8_t shellOpen : 1;
        uint8_t read : 1;
        uint8_t seek : 1;
        uint8_t play : 1;
    };

    uint8_t _value;
} CDROMStatusCode;

/*
	Setmode
	0   CDDA        (0=Off, 1=Allow to Read CD-DA Sectors; ignore missing EDC)
	1   AutoPause   (0=Off, 1=Auto Pause upon End of Track) ;for Audio Play
	2   Report      (0=Off, 1=Enable Report-Interrupts for Audio Play)
	3   XA-Filter   (0=Off, 1=Process only XA-ADPCM sectors that match Setfilter)
	4   Ignore Bit  (0=Normal, 1=Ignore Sector Size and Setloc position)
	5   Sector Size (0=800h=DataOnly, 1=924h=WholeSectorExceptSyncBytes)
	6   XA-ADPCM    (0=Off, 1=Send XA-ADPCM sectors to SPU Audio Input)
	7   Speed       (0=Normal speed, 1=Double speed)
*/
typedef union {
	struct {
		uint8_t CDDASectorsReadEnable : 1;
        uint8_t endOfTrackAutoPauseEnable : 1;
        uint8_t reportInterruptsForAudioPlayEnable : 1;
        uint8_t XAFilterEnable : 1;
        uint8_t unknown : 1;
        uint8_t _sectorSize : 1;
        uint8_t XAADPCMEnable : 1;
        uint8_t _speed : 1;
    };

	uint8_t _value;
} CDROMMode;

/*
    Mode2/Form1 (CD-XA)
    000h 0Ch  Sync
    00Ch 4    Header (Minute,Second,Sector,Mode=02h)
    010h 4    Sub-Header (File, Channel, Submode AND DFh, Codinginfo)
    014h 4    Copy of Sub-Header
    018h 800h Data (2048 bytes)
    818h 4    EDC (checksum accross [010h..817h])
    81Ch 114h ECC (error correction codes)
*/
typedef struct {
    uint8_t sync[12];
    uint8_t header[4];
    uint8_t subheader[4];
    uint8_t subheaderCopy[4];
    uint8_t data[2048];
    uint8_t EDC[4];
    uint8_t ECC[276];
} CDSector;

typedef enum {
    IdleState,
    SeekingState,
    ReadingState,
    ReadingTableOfContentsState,
} CDROMInternalState;

typedef struct
{
	CDROMStatus status;
	CDROMInterrupt interrupt;
	CDROMStatusCode statusCode;
	CDROMMode mode;
	CDROMInterruptFlag interruptFlag;
	CDROMInternalState internalState;

	uint8_t m_interrupt_flag_register;

	// FIFOs

	// Parameter FIFO
	uint8_t m_parameter_fifo[16];
	int8_t m_parameter_fifo_front;
	int8_t m_parameter_fifo_rear;
	int8_t m_parameter_fifo_count;

	// Response FIFO
	uint8_t m_response_fifo[16];
	int8_t m_response_fifo_front;
	int8_t m_response_fifo_rear;
	int8_t m_response_fifo_count;

	
	uint8_t m_interrupt_fifo[64];
	int8_t m_interrupt_fifo_front;
	int8_t m_interrupt_fifo_rear;
	int8_t m_interrupt_fifo_count;

	int8_t m_queued_responses;

	uint32_t m_seek_sector;
	uint32_t m_read_sector;

	uint64_t m_count;

    CDSector currentSector;
    uint8_t readBuffer[0x920];
    uint32_t readBufferIndex;

} m_psx_cdrom_t;

typedef struct {
	uint8_t  id[8];
	uint32_t textOffset;
	uint32_t dataOffset;
	uint32_t pc;
	uint32_t gp;
	uint32_t textAddr;
	uint32_t textSize;
	uint32_t dataAddr;
	uint32_t dataSize;
	uint32_t bssAddr;
	uint32_t bssSize;
	uint32_t stackAddr;
	uint32_t stackSize;
	uint32_t savedSP;
	uint32_t savedFP;
	uint32_t savedGP;
	uint32_t savedRA;
	uint32_t savedS0;
} SCEheader_t;

typedef struct {
	uint8_t  id[8];
	uint32_t textOffset;
	uint32_t pc;
	uint32_t textAddr;
	uint32_t textSize;
	uint32_t stackAddr;
	uint32_t stackSize;
	uint32_t savedSP;
	uint32_t savedFP;
	uint32_t savedGP;
	uint32_t savedRA;
	uint32_t savedS0;
} PSXheader_t;

typedef struct {
	uint8_t id[8];
	uint8_t pad1[8];
	uint32_t pc;
	uint32_t gp;
	uint32_t dst;
	uint32_t size;
	uint32_t pad2[2];
	uint32_t memfillAddr;
	uint32_t memfillSize;
	uint32_t sp;
	uint32_t spOffset;
	uint8_t pad3[20];
	uint8_t SCEI[1972];
	uint8_t data[];
} EXEheader_t;

typedef struct context
{
	uint32_t cycles;
	bool stop;
} context_t;

typedef enum {
    OPENGL,
    VULKAN
} renderer_backends;

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

	m_psx_cdrom_t *m_cdrom;

	uint8_t *exename;
	bool m_sideload;
	bool m_tty;

	bool m_vramview;

	bool m_memory_state;
	bool m_dma_state;
	bool m_interrupts_state;
	bool m_cpu_state;
	bool m_gpu_state;
	bool m_gpu_command_buffer_state;
	bool m_cdrom_state;context_t ctx;

	bool m_debugger;

#ifdef GDBSTUB_SUPPORT
	context_t m_gdbstub_context;
    gdbstub_config_t m_gdbstub_config;
	gdbstub_t *m_gdb;
#endif
	uint32_t m_breakpoint;
	uint32_t m_wp;

#ifdef DUMP_VRAM
	uint8_t *m_vram_data;
#endif

	bool m_cond;

	renderer_backends renderer;

	vulcano_struct *vulcano_state;

    // Scheduled events number
	uint64_t scheduled;
	
    // Master cycles
    uint64_t time;

	char *cd_name;

	bool m_cdrom_in;

	m_psx_gte_t *m_gte;

} m_simplestation_state;

typedef void (*sched_func)(m_simplestation_state *m_simplestation);

/* Structures */
typedef struct
{
    // In how many cycles we have to execute
    uint64_t time;

    char subsystem[8];
	
	// Function to execute
    sched_func func;

} event_t;

// Exception types
typedef enum {
	interrupt = 0x0,
	load_error = 0x4,
	store_error = 0x5,
	_syscall = 0x8,
	breakpoint = 0x9,
	illegal = 0xA,
	coprocessor = 0xB,
	overflow = 0xC
} m_exc_types;

// Interrupt types
typedef enum {
	VBLANK = 0,
	GPU = 1,
	CDROM = 2,
	DMA = 3,
	TMR0 = 4,
	TMR1 = 5,
	TMR2 = 6,
	CONTROLLERMEMCARD = 7,
	SIO = 8,
	SPU = 9,
	LIGHTPEN = 10
} m_int_types;

/* Functions */
uint8_t m_simplestation_exit(m_simplestation_state *m_simplestation, uint8_t m_is_fatal);
extern void m_printregs(m_simplestation_state *m_simplestation);

#endif /* SIMPLESTATION_H */
