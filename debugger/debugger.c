#include <debugger/debugger.h>
#include <cpu/instructions.h>
#include <cpu/instructions_debug.h>

#ifdef GDBSTUB_SUPPORT

#define GDBSTUB_IMPLEMENTATION
// Uncomment to enable GDBSTUB debug output
// #define GDBSTUB_DEBUG
#include <extern/gdbstub/gdbstub.h>

#include <byteswap.h>

#endif

void m_printregs(m_simplestation_state *m_simplestation)
{
	printf(MAGENTA UNDERSCORE "\nGeneral Prupose Registers:\n" WHITE NORMAL);

	for (size_t m_idx = 0; m_idx < M_R3000_REGISTERS; m_idx++)
	{
		printf(MAGENTA "%s:" NORMAL WHITE " 0x%08X ", m_cpu_regnames[m_idx], m_simplestation->m_cpu->m_registers[m_idx]);
		
		if (m_idx % 2)
		{
			printf("\n");
		}
	}

	printf(BLUE UNDERSCORE "\nControl-Flow Registers:\n" WHITE NORMAL);

	if (m_simplestation->m_cpu->m_branch == true)
	{
		printf(BLUE "PC:" WHITE " 0x%08X   " GREEN "[DS]" NORMAL "\n", m_simplestation->m_cpu->m_pre_ds_pc);
	}
	else
	{
		printf(BLUE "PC:" WHITE " 0x%08X\n", PC - 4);
	}

	printf(GREEN UNDERSCORE "\nArithmetic Registers:\n" WHITE NORMAL);
	printf(GREEN "HI:" WHITE " 0x%08X\n", HI);
	printf(GREEN "LO:" WHITE " 0x%08X\n", LO);

	printf(YELLOW UNDERSCORE "\nCOP0:\n" WHITE NORMAL);
	printf(YELLOW "SR    : " WHITE " 0x%08X\n", COP0_SR);
	printf(YELLOW "EPC   : " WHITE " 0x%08X\n", COP0_EPC);
	printf(YELLOW "CAUSE : " WHITE " 0x%08X\n", COP0_CAUSE);
	printf("\n");
}

#ifndef GDBSTUB_SUPPORT
void m_debugger(m_simplestation_state *m_simplestation)
{
	bool m_cond = true;

	while (m_cond)
	{
		printf("\e[1;1H\e[2J");
	
		if (m_simplestation->m_cpu->m_branch == true)
		{
			printf(NORMAL "PC     " GREEN "[DS]" NORMAL " : 0x%08X\n\nOpcode      : 0x%08X\n", m_simplestation->m_cpu->m_pre_ds_pc, m_simplestation->m_cpu->m_opcode);
		}
		else
		{
			printf(NORMAL "PC          : 0x%08X\n\nOpcode      : 0x%08X\n", PC - 4, m_simplestation->m_cpu->m_opcode);
		}
			
		printf("Disassembly : ");

		if (m_psx_instrs_opcodes[INSTRUCTION].m_funct == NULL)
		{
			printf(RED "[CPU] fde: Illegal Opcode: 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, REGIDX_S, m_simplestation->m_cpu->m_opcode);
			m_exc_types m_exc = illegal;
			m_exception(m_exc, m_simplestation);
		}
		else
		{
			// Execute the instruction
			((void (*) (m_simplestation_state *m_simplestation))m_psx_instrs_opcodes[INSTRUCTION].m_funct)(m_simplestation);
		}
			
		m_printregs(m_simplestation);

		m_simplestation->m_cpu->m_branch = false;
		m_simplestation->m_cpu->m_pre_ds_pc = 0;

		printf("\nPress ENTER to step\n");

		getchar();
		
		// Check if the instruction is implemented
		if (m_psx_instrs[INSTRUCTION].m_funct == NULL)
		{
			m_exc_types m_exc = illegal;
			m_exception(m_exc, m_simplestation);
		}
		else
		{
			// Execute the instruction
			((void (*) (m_simplestation_state *m_simplestation))m_psx_instrs[INSTRUCTION].m_funct)(m_simplestation);
		}

		if (m_simplestation->m_cpu->m_branch)
		{
			m_simplestation->m_cpu->m_pre_ds_pc = PC;
		}

		m_cpu_delay_slot_handler(m_simplestation);
		m_simplestation->m_cpu->m_opcode = m_simplestation->m_cpu->m_next_opcode;

		PC = NXT_PC;

		/* Fetch cycle */
		m_simplestation->m_cpu->m_next_opcode = m_memory_read((PC), dword, m_simplestation);
			
		// Increment Program Counter by 4
		NXT_PC += 4;
	}
}
#else

const char TARGET_CONFIG[] = R"(<?xml version="1.0"?>
<!DOCTYPE feature SYSTEM "gdb-target.dtd">
<target version="1.0">
<!-- Helping GDB -->
<architecture>mips:3000</architecture>
<osabi>none</osabi>
<!-- Mapping ought to be flexible, but there seems to be some
     hardcoded parts in gdb, so let's use the same mapping. -->
<feature name="org.gnu.gdb.mips.cpu">
  <reg name="r0" bitsize="32" regnum="0"/>
  <reg name="r1" bitsize="32"/>
  <reg name="r2" bitsize="32"/>
  <reg name="r3" bitsize="32"/>
  <reg name="r4" bitsize="32"/>
  <reg name="r5" bitsize="32"/>
  <reg name="r6" bitsize="32"/>
  <reg name="r7" bitsize="32"/>
  <reg name="r8" bitsize="32"/>
  <reg name="r9" bitsize="32"/>
  <reg name="r10" bitsize="32"/>
  <reg name="r11" bitsize="32"/>
  <reg name="r12" bitsize="32"/>
  <reg name="r13" bitsize="32"/>
  <reg name="r14" bitsize="32"/>
  <reg name="r15" bitsize="32"/>
  <reg name="r16" bitsize="32"/>
  <reg name="r17" bitsize="32"/>
  <reg name="r18" bitsize="32"/>
  <reg name="r19" bitsize="32"/>
  <reg name="r20" bitsize="32"/>
  <reg name="r21" bitsize="32"/>
  <reg name="r22" bitsize="32"/>
  <reg name="r23" bitsize="32"/>
  <reg name="r24" bitsize="32"/>
  <reg name="r25" bitsize="32"/>
  <reg name="r26" bitsize="32"/>
  <reg name="r27" bitsize="32"/>
  <reg name="r28" bitsize="32"/>
  <reg name="r29" bitsize="32"/>
  <reg name="r30" bitsize="32"/>
  <reg name="r31" bitsize="32"/>
  <reg name="lo" bitsize="32" regnum="33"/>
  <reg name="hi" bitsize="32" regnum="34"/>
  <reg name="pc" bitsize="32" regnum="37"/>
</feature>
<feature name="org.gnu.gdb.mips.cp0">
  <reg name="status" bitsize="32" regnum="32"/>
  <reg name="badvaddr" bitsize="32" regnum="35"/>
  <reg name="cause" bitsize="32" regnum="36"/>
</feature>
<!-- We don't have an FPU, but gdb hardcodes one, and will choke
     if this section isn't present. -->
<feature name="org.gnu.gdb.mips.fpu">
  <reg name="f0" bitsize="32" type="ieee_single" regnum="38"/>
  <reg name="f1" bitsize="32" type="ieee_single"/>
  <reg name="f2" bitsize="32" type="ieee_single"/>
  <reg name="f3" bitsize="32" type="ieee_single"/>
  <reg name="f4" bitsize="32" type="ieee_single"/>
  <reg name="f5" bitsize="32" type="ieee_single"/>
  <reg name="f6" bitsize="32" type="ieee_single"/>
  <reg name="f7" bitsize="32" type="ieee_single"/>
  <reg name="f8" bitsize="32" type="ieee_single"/>
  <reg name="f9" bitsize="32" type="ieee_single"/>
  <reg name="f10" bitsize="32" type="ieee_single"/>
  <reg name="f11" bitsize="32" type="ieee_single"/>
  <reg name="f12" bitsize="32" type="ieee_single"/>
  <reg name="f13" bitsize="32" type="ieee_single"/>
  <reg name="f14" bitsize="32" type="ieee_single"/>
  <reg name="f15" bitsize="32" type="ieee_single"/>
  <reg name="f16" bitsize="32" type="ieee_single"/>
  <reg name="f17" bitsize="32" type="ieee_single"/>
  <reg name="f18" bitsize="32" type="ieee_single"/>
  <reg name="f19" bitsize="32" type="ieee_single"/>
  <reg name="f20" bitsize="32" type="ieee_single"/>
  <reg name="f21" bitsize="32" type="ieee_single"/>
  <reg name="f22" bitsize="32" type="ieee_single"/>
  <reg name="f23" bitsize="32" type="ieee_single"/>
  <reg name="f24" bitsize="32" type="ieee_single"/>
  <reg name="f25" bitsize="32" type="ieee_single"/>
  <reg name="f26" bitsize="32" type="ieee_single"/>
  <reg name="f27" bitsize="32" type="ieee_single"/>
  <reg name="f28" bitsize="32" type="ieee_single"/>
  <reg name="f29" bitsize="32" type="ieee_single"/>
  <reg name="f30" bitsize="32" type="ieee_single"/>
  <reg name="f31" bitsize="32" type="ieee_single"/>
  <reg name="fcsr" bitsize="32" group="float"/>
  <reg name="fir" bitsize="32" group="float"/>
</feature>
</target>
)";

const char MEMORY_MAP[] = R"(<?xml version="1.0"?>
<memory-map>
  <!-- Everything here is described as RAM, because we don't really
       have any better option. -->
  <!-- Main memory bloc: let's go with 8MB straight off the bat. -->
  <memory type="ram" start="0x0000000000000000" length="0x800000"/>
  <memory type="ram" start="0xffffffff80000000" length="0x800000"/>
  <memory type="ram" start="0xffffffffa0000000" length="0x800000"/>
  <!-- EXP1 can go up to 8MB too. -->
  <memory type="ram" start="0x000000001f000000" length="0x800000"/>
  <memory type="ram" start="0xffffffff9f000000" length="0x800000"/>
  <memory type="ram" start="0xffffffffbf000000" length="0x800000"/>
  <!-- Scratchpad -->
  <memory type="ram" start="0x000000001f800000" length="0x400"/>
  <memory type="ram" start="0xffffffff9f800000" length="0x400"/>
  <!-- Hardware registers -->
  <memory type="ram" start="0x000000001f801000" length="0x2000"/>
  <memory type="ram" start="0xffffffff9f801000" length="0x2000"/>
  <memory type="ram" start="0xffffffffbf801000" length="0x2000"/>
  <!-- DTL BIOS SRAM -->
  <memory type="ram" start="0x000000001fa00000" length="0x200000"/>
  <memory type="ram" start="0xffffffff9fa00000" length="0x200000"/>
  <memory type="ram" start="0xffffffffbfa00000" length="0x200000"/>
  <!-- BIOS -->
  <memory type="ram" start="0x000000001fc00000" length="0x80000"/>
  <memory type="ram" start="0xffffffff9fc00000" length="0x80000"/>
  <memory type="ram" start="0xffffffffbfc00000" length="0x80000"/>
  <!-- This really is only for 0xfffe0130 -->
  <memory type="ram" start="0xfffffffffffe0000" length="0x200"/>
</memory-map>
)";

m_simplestation_state *m_simplestation_debug;

void gdb_connected(context_t * ctx)
{
    printf("Connected\n");
}

void gdb_disconnected(context_t * ctx)
{
    printf("Disconnected\n");
}

void gdb_start(context_t * ctx)
{
	ctx->stop = false;
}

void gdb_stop(context_t * ctx)
{
    printf("Stopping...\n");
	ctx->stop = true;
}

void gdb_step(context_t * ctx)
{
	m_cpu_fde(m_simplestation_debug);

	ctx->cycles++;
}

void gdb_set_breakpoint(context_t * ctx, uint32_t address)
{
    printf("Set breakpoint %08X\n", address);
}

void gdb_clear_breakpoint(context_t * ctx, uint32_t address)
{
    printf("Clear breakpoint %08X\n", address);
}

ssize_t gdb_get_memory(context_t * ctx, char * buffer, size_t buffer_length, uint32_t address, size_t length)
{
	// 32
	if (length >= 4)
	{
        return snprintf(buffer, buffer_length, "%08lx", __bswap_32(m_memory_read(address, dword, m_simplestation_debug)));
    }
	else if(length == 2) // 16
	{
        return snprintf(buffer, buffer_length, "%04lx", __bswap_32(m_memory_read(address, word, m_simplestation_debug)));
    }
	else if(length == 1) // 8
	{
        return snprintf(buffer, buffer_length, "%02lx", __bswap_32(m_memory_read(address, byte, m_simplestation_debug)));
    }

    return snprintf(buffer, buffer_length, "00000000");
}

ssize_t gdb_get_register_value(context_t * ctx, char * buffer, size_t buffer_length, int reg)
{
	// COP0 Status Register
	if (reg == 33)
	{
		return snprintf(buffer, buffer_length, "%08x", __bswap_32(m_simplestation_debug->m_cpu_cop0->m_registers[12]));
	}
	else if (reg == 33) // LO Arithmetic Register
	{
		return snprintf(buffer, buffer_length, "%08x", __bswap_32(m_simplestation_debug->m_cpu->m_lo));
	}
	else if (reg == 34) // HI Arithmetic Register
	{
		return snprintf(buffer, buffer_length, "%08x", __bswap_32(m_simplestation_debug->m_cpu->m_hi));
	}
	else if (reg == 35) // COP0 EPC Register
	{
		return snprintf(buffer, buffer_length, "%08x", __bswap_32(m_simplestation_debug->m_cpu_cop0->m_registers[14]));
	}
	else if (reg == 36) // COP0 Cause Register
	{
		return snprintf(buffer, buffer_length, "%08x", __bswap_32(m_simplestation_debug->m_cpu_cop0->m_registers[13]));
	}
	else if (reg == 37) // PC
	{
		return snprintf(buffer, buffer_length, "%08x", __bswap_32(m_simplestation_debug->m_cpu->m_pc));
	}

    return snprintf(buffer, buffer_length, "%08x", __bswap_32(m_simplestation_debug->m_cpu->m_registers[reg]));
}

ssize_t gdb_get_general_registers(context_t * ctx, char * buffer, size_t buffer_length)
{
    return snprintf(buffer, buffer_length, "00000000");
}

bool at_breakpoint()
{
    // Detect breakpoint logic
    return false;
}

void m_init_gdbstub(m_simplestation_state *m_simplestation)
{
	m_simplestation_debug = m_simplestation;
	m_simplestation->m_gdbstub_context.cycles = 0;
	m_simplestation->m_gdbstub_context.stop = true;

    m_simplestation->m_gdbstub_config.port = 5678;
    m_simplestation->m_gdbstub_config.user_data = &m_simplestation->m_gdbstub_context;
    m_simplestation->m_gdbstub_config.connected = (gdbstub_connected_t)gdb_connected;
    m_simplestation->m_gdbstub_config.disconnected = (gdbstub_disconnected_t)gdb_disconnected;
    m_simplestation->m_gdbstub_config.start = (gdbstub_start_t)gdb_start;
    m_simplestation->m_gdbstub_config.stop = (gdbstub_stop_t)gdb_stop;
    m_simplestation->m_gdbstub_config.step = (gdbstub_step_t)gdb_step;
    m_simplestation->m_gdbstub_config.set_breakpoint = (gdbstub_set_breakpoint_t)gdb_set_breakpoint;
    m_simplestation->m_gdbstub_config.clear_breakpoint = (gdbstub_clear_breakpoint_t)gdb_clear_breakpoint;
    m_simplestation->m_gdbstub_config.get_memory = (gdbstub_get_memory_t)gdb_get_memory;
    m_simplestation->m_gdbstub_config.get_register_value = (gdbstub_get_register_value_t)gdb_get_register_value;
    m_simplestation->m_gdbstub_config.get_general_registers = (gdbstub_get_general_registers_t)gdb_get_general_registers;
    m_simplestation->m_gdbstub_config.target_config = TARGET_CONFIG;
    m_simplestation->m_gdbstub_config.target_config_length = sizeof(TARGET_CONFIG);
    m_simplestation->m_gdbstub_config.memory_map = MEMORY_MAP;
    m_simplestation->m_gdbstub_config.memory_map_length = sizeof(MEMORY_MAP);

    m_simplestation->m_gdb = gdbstub_init(m_simplestation->m_gdbstub_config);
    
    if (!m_simplestation->m_gdb) {
        printf("failed to create gdbstub\n");
        return 1;
    }
}
#endif
