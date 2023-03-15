#include <cpu/gte.h>

uint8_t gte_init(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gte = malloc(sizeof(m_psx_gte_t));
}

uint8_t gte_exit(m_simplestation_state *m_simplestation)
{
    free(m_simplestation->m_gte);
}

void gte_execute(uint32_t opcode, m_simplestation_state *m_simplestation)
{
	switch (opcode & 0x3F)
	{
		case 0x00:
			// TODO: Properly implement the opcode
			break;
			
		default:
			printf(RED "[CPU] gte: Unimplemented 'GTE' Opcode: 0x%08X\n" NORMAL, opcode & 0x3F);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}
