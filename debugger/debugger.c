#include <debugger/debugger.h>
#include <cpu/instructions.h>
#include <cpu/instructions_debug.h>

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
