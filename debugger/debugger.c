#include <debugger/debugger.h>
#include <cpu/instructions.h>
#include <cpu/opcodes.h>

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
	printf(BLUE "PC:" WHITE " 0x%08X\n", PC);

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
	bool m_cond = true, m_firstrun = true;
	
	while (m_cond)
	{
		if (m_firstrun)
		{
			m_firstrun = false;
		}
		else
		{
			m_simplestation->m_cpu->m_opcode = m_simplestation->m_cpu->m_next_opcode;

			PC = NXT_PC;

			/* Fetch cycle */
			m_simplestation->m_cpu->m_next_opcode = m_memory_read((PC), dword, m_simplestation);
			
			// Increment Program Counter by 4
			NXT_PC += 4;
		}
		
		// Check if the instruction is implemented
		if (m_psx_instrs[INSTRUCTION].m_funct == NULL)
		{
			printf(RED "[CPU] fde: Unimplemented Instruction 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, INSTRUCTION, m_simplestation->m_cpu->m_opcode);
			m_simplestation_exit(m_simplestation, 1);
		}
		else
		{
			// Execute the instruction
			((void (*) (m_simplestation_state *m_simplestation))m_psx_instrs[INSTRUCTION].m_funct)(m_simplestation);
		}

		m_cpu_delay_slot_handler(m_simplestation);

		while (1)
		{
			printf("\e[1;1H\e[2J");
	
			if (NXT_PC > (PC + 4))
			{
				printf("PC " GREEN "[DS]" NORMAL ": 0x%08X\nOpcode : 0x%08X\n\n", PC, m_simplestation->m_cpu->m_opcode);
			}
			else
			{
				printf("PC     : 0x%08X\nOpcode : 0x%08X\n\n", PC, m_simplestation->m_cpu->m_opcode);
			}
			
			if (m_psx_instrs_opcodes[INSTRUCTION].m_funct == NULL)
			{
				printf(RED "[CPU] fde: Unimplemented Instruction 0x%02X (Full Opcode: 0x%08X)\n" NORMAL, INSTRUCTION, m_simplestation->m_cpu->m_opcode);
				m_simplestation_exit(m_simplestation, 1);
			}
			else
			{
				// Execute the instruction
				((void (*) (m_simplestation_state *m_simplestation))m_psx_instrs_opcodes[INSTRUCTION].m_funct)(m_simplestation);
			}
			
			m_printregs(m_simplestation);

			printf("\nPress ENTER to step\n");

			getchar();

			break;
		}
	}
}
