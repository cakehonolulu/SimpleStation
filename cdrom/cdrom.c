#include <cdrom/cdrom.h>
#include <cdrom/parameter_fifo.h>
#include <cdrom/response_fifo.h>
#include <cdrom/interrupt_fifo.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

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
		m_cdrom_interrupt_fifo_init(m_simplestation);
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
	m_simplestation->m_cdrom->m_status_register.raw = 0x18;
	m_simplestation->m_cdrom->m_interrupt_enable_register.raw = 0x0;
	m_simplestation->m_cdrom->m_stat.raw = 0x10;
}

void m_cdrom_set_status(uint8_t value, m_simplestation_state *m_simplestation)
{
	// TODO: Check the next line's accuracy
    m_simplestation->m_cdrom->m_status_register.index = value & 0x3;
}

uint8_t m_cdrom_get_status_register(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->m_status_register.raw;
}

void m_cdrom_set_interrupt_register(uint8_t value, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->m_interrupt_enable_register.raw = value;
}

void m_cdrom_set_interrupt_flag_register(uint8_t value, m_simplestation_state *m_simplestation)
{
	if (value & 0x40)
	{
		m_parameter_fifo_flush(m_simplestation);
		m_cdrom_update_status_register(m_simplestation);
	}

	if (!m_cdrom_interrupt_fifo_is_empty(m_simplestation))
	{
		m_cdrom_interrupt_fifo_pop(m_simplestation);
	}
}

uint8_t m_cdrom_get_interrupt_flag_register(m_simplestation_state *m_simplestation)
{
    uint8_t flags = 0b11100000;

    if (!m_cdrom_interrupt_fifo_is_empty(m_simplestation))
	{
        flags |= m_cdrom_interrupt_fifo_front(m_simplestation) & 0x7;
    }

    return flags;
}

void m_cdrom_update_status_register(m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cdrom->m_status_register.prmempt = m_cdrom_parameter_fifo_is_empty(m_simplestation);
    m_simplestation->m_cdrom->m_status_register.prmwrdy = !(m_cdrom_parameter_fifo_size(m_simplestation) >= 16);
    m_simplestation->m_cdrom->m_status_register.rslrrdy = !m_cdrom_response_fifo_is_empty(m_simplestation);
}

void m_cdrom_getstat_cmd(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] GETSTAT" NORMAL "\n");

	if (m_simplestation->m_cdrom_in)
	{
		m_simplestation->m_cdrom->m_stat.shellOpen = 0;
		m_cdrom_response_fifo_push(m_simplestation->m_cdrom->m_stat.raw, m_simplestation);
	}
	else
	{
		m_simplestation->m_cdrom->m_stat.shellOpen = 1;
		m_cdrom_response_fifo_push(m_simplestation->m_cdrom->m_stat.raw, m_simplestation);
	}

    m_cdrom_interrupt_fifo_push(0x3, m_simplestation);
}

void m_cdrom_test_cmd(m_simplestation_state *m_simplestation)
{
	uint8_t sub_cmd = m_cdrom_parameter_fifo_front(m_simplestation);

	switch (sub_cmd)
	{
		case INT3_yy_mm_dd_ver:
			printf(BOLD MAGENTA"[CDROM] test_cmd: INT3(yy,mm,dd)" NORMAL "\n");
			m_cdrom_response_fifo_push(0x94, m_simplestation);
            m_cdrom_response_fifo_push(0x09, m_simplestation);
            m_cdrom_response_fifo_push(0x19, m_simplestation);
            m_cdrom_response_fifo_push(0xC0, m_simplestation);
			m_cdrom_interrupt_fifo_push(0x3, m_simplestation);
			break;

		default:
			printf(RED "[CDROM] test_cmd: Unhandled CDROM sub-command 0x%X" NORMAL "\n", sub_cmd);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}

void m_cdrom_getid_cmd(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA"[CDROM] GETID" NORMAL "\n");
	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->m_stat.raw, m_simplestation);
    m_cdrom_response_fifo_push(0x00, m_simplestation);
    m_cdrom_response_fifo_push(0x20, m_simplestation);
    m_cdrom_response_fifo_push(0x00, m_simplestation);
    m_cdrom_response_fifo_push('S', m_simplestation);
    m_cdrom_response_fifo_push('C', m_simplestation);
    m_cdrom_response_fifo_push('E', m_simplestation);
    m_cdrom_response_fifo_push('A', m_simplestation);
    m_cdrom_interrupt_fifo_push(0x2, m_simplestation);
}

void m_cdrom_execute(uint8_t value, m_simplestation_state *m_simplestation)
{
	printf("[CDROM] execute: Command recieved\n");

	m_interrupt_fifo_flush(m_simplestation);
    m_response_fifo_flush(m_simplestation);

	switch (value)
	{
		case CDROM_GETSTAT_CMD:
			m_cdrom_getstat_cmd(m_simplestation);
			break;

		case CDROM_TEST_CMD:
			m_cdrom_test_cmd(m_simplestation);
			break;

		case CDROM_GETID_CMD:
			m_cdrom_getid_cmd(m_simplestation);
			break;

		default:
			printf(RED "[CDROM] execute: Unhandled CDROM command 0x%02X" NORMAL "\n", value);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}

	m_parameter_fifo_flush(m_simplestation);
	m_cdrom_update_status_register(m_simplestation);
}

void m_cdrom_tick(m_simplestation_state *m_simplestation)
{
	if (!m_cdrom_interrupt_fifo_is_empty(m_simplestation))
	{
		if ((m_simplestation->m_cdrom->m_interrupt_enable_register.raw & 0x7) & (m_cdrom_interrupt_fifo_front(m_simplestation) & 0x7))
		{
			printf("[CDROM] tick: Triggering CDROM interrupt...\n");
			m_interrupts_trigger(CDROM, m_simplestation);
		}
	}
}

void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    switch (m_offset)
    {
		// Index/Status Register
		case 0:
			// Get least significant byte
			m_cdrom_set_status(m_value & 0xFF, m_simplestation);
			printf("[CDROM] write: STATUS Register, written value (0x%X)\n", m_value & 0xFF);
			break;

		case 1:
			m_cdrom_execute(m_value, m_simplestation);
			break;

		// Status's Index-determined write
		case 2:
			switch (m_simplestation->m_cdrom->m_status_register.index)
			{
				// Parameter FIFO Push
				case 0:
					m_cdrom_parameter_fifo_push(m_value, m_simplestation);
					printf("[CDROM] write: Parameter FIFO, written value (0x%X)\n", m_value);
					break;

				// Interrupt Enable Register Write
				case 1:
					m_cdrom_set_interrupt_register(m_value, m_simplestation);
					printf("[CDROM] write: Interrupt Register, written value (0x%X)\n", m_value);
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
					m_cdrom_set_interrupt_flag_register(m_value, m_simplestation);
					printf("[CDROM] write: Interrupt Flag Register, written value (0x%X)\n", m_value);
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
		// Status Register
		case 0:
			m_value = m_cdrom_get_status_register(m_simplestation);
			printf("[CDROM] read: STATUS Register, value: 0x%X\n", m_value);
			break;

		case 1:
			switch (m_simplestation->m_cdrom->m_status_register.index)
			{
				case 1:
					m_value = m_cdrom_response_fifo_front(m_simplestation);
					m_cdrom_response_fifo_pop(m_simplestation);
					break;

				default:
					printf(RED "[CDROM] write: Unhandled Offset 1 CDROM Read (Index: %d)\n" NORMAL,
							m_simplestation->m_cdrom->m_status_register.index);
					m_simplestation_exit(m_simplestation, 1);
					break;
			}
			break;

		case 3:
			switch (m_simplestation->m_cdrom->m_status_register.index)
			{
				case 1:
					m_value = m_cdrom_get_interrupt_flag_register(m_simplestation);
					printf("[CDROM] read: Interrupt Flag Register, value: 0x%X\n", m_value);
					break;

				default:
					printf(RED "[CDROM] write: Unhandled Offset 3 CDROM Read (Index: %d)\n" NORMAL,
							m_simplestation->m_cdrom->m_status_register.index);
					m_simplestation_exit(m_simplestation, 1);
					break;
			}
			break;


        default:
            printf(RED "[CDROM] read: Unhandled CDROM Read (Offset: %d)\n" NORMAL, m_offset);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

    return m_value;
}
