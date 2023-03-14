#include <cdrom/cdrom.h>
#include <cdrom/parameter_fifo.h>
#include <cdrom/response_fifo.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

uint8_t queued_fifo[16] = { 0 };

uint8_t m_cdrom_init(m_simplestation_state *m_simplestation)
{
    uint8_t m_result = 0;

    m_simplestation->m_cdrom = (m_psx_cdrom_t *) malloc(sizeof(m_psx_cdrom_t));

    if (m_simplestation->m_cdrom)
    {
        m_simplestation->m_cdrom_state = ON;
        memset(m_simplestation->m_cdrom, 0, sizeof(m_psx_cdrom_t));
		m_cdrom_setup(m_simplestation);
		m_cdrom_parameter_fifo_init(m_simplestation);
		m_cdrom_response_fifo_init(m_simplestation);
    }
    else
    {
        m_result = 1;
    }

    return m_result;
}

void m_cdrom_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom)
    {
        free(m_simplestation->m_cdrom);
    }
}

void m_cdrom_setup(m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cdrom->m_status_register.prmempt = 1;
	m_simplestation->m_cdrom->m_status_register.prmwrdy = 1;
	m_simplestation->m_cdrom->m_status_register.rslrrdy = 1;
	m_simplestation->m_cdrom->m_queued_responses = -1;
}

void m_cdrom_step(m_simplestation_state *m_simplestation)
{
	if (m_simplestation->m_cdrom->m_queued_responses >= 0)
	{
		m_simplestation->m_cpu_ints->m_interrupt_stat |= 0x04;
		m_simplestation->m_cdrom->m_interrupt_flag_register = m_simplestation->m_cdrom->m_queued_responses;
		// FIXME?
		//m_exception(0x01 , m_simplestation);
		m_simplestation->m_cdrom->m_queued_responses = -1;
	}
}

void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    switch (m_offset)
    {
		// Index/Status Register
		case 0:
			m_simplestation->m_cdrom->m_status_register.index = m_value;
			break;

		// Command Register Write
		case 1:
			m_cdrom_exec_cmd(m_value, m_simplestation);
			break;

		// Status's Index-determined write
		case 2:
			switch (m_simplestation->m_cdrom->m_status_register.index)
			{
				// Parameter FIFO Param Write
				case 0:
					m_cdrom_parameter_fifo_push(m_value, m_simplestation);
					break;
				
				// Interrupt Enable Register Write
				case 1:
					m_simplestation->m_cdrom->m_interrupt_enable_register = m_value;
					break;

				default:
					printf(RED "[CDROM] write: Unhandled Offset 2 CDROM Write (Index: %d, Value: 0x%08X)\n" NORMAL,
							m_simplestation->m_cdrom->m_status_register.index, m_value);
					m_simplestation_exit(m_simplestation, 1);
            		break;
			}
			break;

		// Status's Index-determined write
		case 3:
			switch (m_simplestation->m_cdrom->m_status_register.index)
			{
				// Interrupt Flag Register Write
				case 1:
					m_simplestation->m_cdrom->m_interrupt_flag_register &= ~m_value;
					break;

				default:
					printf(RED "[CDROM] write: Unhandled Offset 3 CDROM Write (Index: %d, Value: 0x%08X)\n" NORMAL,
							m_simplestation->m_cdrom->m_status_register.index, m_value);
					m_simplestation_exit(m_simplestation, 1);
            		break;
			}
			break;

        default:
            printf(RED "[CDROM] write: Unhandled CDROM Write (Offset: %d, Value: 0x%08X)\n" NORMAL, m_offset, m_value);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }
}

uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation)
{
    uint32_t m_value = 0;

	// Available CDROM Ports range from 0-3 
    switch (m_offset)
    {
		// Index/Status Register
		case 0:
			m_value = m_simplestation->m_cdrom->m_status_register.raw;
			break;

		// Response FIFO Read
		case 1:
			m_value = m_cdrom_response_fifo_pop(m_simplestation);
			break;

		case 3:
			m_value = m_simplestation->m_cdrom->m_interrupt_flag_register | 0xE0;
			break;

        default:
            printf(RED "[CDROM] read: Unhandled CDROM Read (Offset: %d)\n" NORMAL, m_offset);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

    return m_value;
}

void INT2(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] INT2" NORMAL "\n");
	m_simplestation->m_cdrom->m_interrupt_flag_register &= ~0b111;
	m_simplestation->m_cdrom->m_interrupt_flag_register |= 0b010;

	for (int i = 0; i < 16; i++)
	{
		m_simplestation->m_cdrom->m_response_fifo[i] = queued_fifo[i];
		queued_fifo[i] = 0;
	}
	m_simplestation->m_cdrom->m_response_fifo_index = 0;
	
	m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;
	m_interrupts_request(CDROM, m_simplestation);
}

void INT3(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] INT3" NORMAL "\n");
	m_simplestation->m_cdrom->m_interrupt_flag_register &= ~0b111;
	m_simplestation->m_cdrom->m_interrupt_flag_register |= 0b011;
	m_interrupts_request(CDROM, m_simplestation);
}

void m_cdrom_exec_cmd(uint8_t m_cmd, m_simplestation_state *m_simplestation)
{
	switch(m_cmd)
	{
		case CDROM_SETLOC_CMD:
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;
			
			uint8_t mm = m_cdrom_parameter_fifo_pop(m_simplestation);
			uint8_t ss = m_cdrom_parameter_fifo_pop(m_simplestation);
			uint8_t ff = m_cdrom_parameter_fifo_pop(m_simplestation);

			mm = (mm - 6 * (mm >> 4));
			ss = (ss - 6 * (ss >> 4));
			ff = (ff - 6 * (ff >> 4));

			uint16_t seekloc = ff + (ss * 75) + (mm * 60 * 75);
			printf("[CDROM] SetLoc (%d;%d;%d %d)\n", mm, ss, ff, seekloc);
			m_cdrom_response_fifo_push(m_simplestation->m_cdrom->m_status_register.raw, m_simplestation);
			event_t event3;
			event3.time = m_simplestation->time + 18491;
			event3.func = &INT3;
			strcpy(event3.subsystem, "CDROM");
			scheduler_push(event3, m_simplestation);
			printf("Created event CDROM_SETLOC_CMD...\n");

			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;
			break;

		case CDROM_SEEKL_CMD:
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;
			printf("[CDROM] SeekL\n");
			m_cdrom_response_fifo_push(0x42, m_simplestation);
			queued_fifo[0] = m_simplestation->m_cdrom->m_status_register.raw;
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;

			event_t event4;
			event4.time = m_simplestation->time + 50401;
			event4.func = &INT3;
			strcpy(event4.subsystem, "CDROM");
			scheduler_push(event4, m_simplestation);

			event_t event5;
			event5.time = m_simplestation->time + 200000;
			event5.func = &INT2;
			strcpy(event5.subsystem, "CDROM");
			scheduler_push(event5, m_simplestation);

			printf("Created event CDROM_SEEKL_CMD...\n");
			break;

		case CDROM_GETSTAT_CMD:
			/*
				HACK:
				
				Returning CDROM STAT w/bit 4 ('ShellOpen') set, enables the
				'no-disk' mode for the shell (Informs the shell that the CDROM 'tray'
				is currently opened == !disk == jump to shell).
			*/
			// 0b00010000
			m_cdrom_response_fifo_push(0b00000000, m_simplestation);
			m_simplestation->m_cdrom->m_queued_responses = 3;
			event_t event;
			event.time = m_simplestation->time + 50401;
			event.func = &INT3;
			strcpy(event.subsystem, "CDROM");
			scheduler_push(event, m_simplestation);
			printf("Created event CDROM_GETSTAT_CMD...\n");
			break;

		case CDROM_TEST_CMD:
			m_cdrom_exec_test_subcmd(m_cdrom_parameter_fifo_pop(m_simplestation), m_simplestation);
			break;

		case CDROM_GETID_CMD:
			m_cdrom_response_fifo_push(m_simplestation->m_cdrom->m_status_register.raw, m_simplestation);
			m_simplestation->m_cdrom->m_queued_responses = 3;
			event_t event2;
			event2.time = m_simplestation->time + 50401 + 18944;
			event2.func = &INT2;
			strcpy(event2.subsystem, "CDROM");
			queued_fifo[0] = 0x02;
			queued_fifo[1] = 0;
			queued_fifo[2] = 0;
			queued_fifo[3] = 0;
			queued_fifo[4] = 'S';
			queued_fifo[5] = 'C';
			queued_fifo[6] = 'E';
			queued_fifo[7] = 'A';
			scheduler_push(event2, m_simplestation);
			printf("Created event CDROM_GETID_CMD...\n");
			break;

		default:
			printf(RED "[CDROM] write: Unhandled CDROM Command: 0x%02X\n" NORMAL, m_cmd);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}

void m_cdrom_exec_test_subcmd(uint8_t m_subcmd, m_simplestation_state *m_simplestation)
{
	switch (m_subcmd)
	{
		// INT3(yy,mm,dd,ver)
		case 0x20:
			// PSX (PU-7) board, to try match SCPH1001
			// TODO: Return different response based on BIOS checksum?
			m_cdrom_response_fifo_push(0xC0, m_simplestation);
			m_cdrom_response_fifo_push(0x94, m_simplestation);
			m_cdrom_response_fifo_push(0x11, m_simplestation);
			m_cdrom_response_fifo_push(0x18, m_simplestation);

			m_simplestation->m_cdrom->m_queued_responses = 3;
			event_t event;
			event.time = m_simplestation->time + 50401;
			event.func = &INT3;
			strcpy(event.subsystem, "CDROM");
			scheduler_push(event, m_simplestation);
			printf("Created event INT3(yy,mm,dd,ver)...\n");
			break;

		default:
			printf(RED "[CDROM] write: Unhandled CDROM TEST Sub-command: 0x%02X\n" NORMAL, m_subcmd);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}
