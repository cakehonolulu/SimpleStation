#include <cdrom/cdrom.h>
#include <cdrom/parameter_fifo.h>
#include <cdrom/response_fifo.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

const uint32_t SecondsPerMinute = 60;
const uint32_t SectorsPerSecond = 75;

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
	m_simplestation->m_cdrom->status._value = 0x18;
	m_simplestation->m_cdrom->interrupt._value = 0x00;
	m_simplestation->m_cdrom->statusCode._value = 0x10;
	m_simplestation->m_cdrom->m_seek_sector = 0;
	m_simplestation->m_cdrom->m_read_sector = 0;
	m_simplestation->m_cdrom->mode._value = 0;
}

/* Helper functions */

void setStatusRegister(uint8_t value, m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cdrom->status.index = value & 0x3;
}

void setInterruptRegister(uint8_t value, m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cdrom->interrupt.enable = value;
}

void setInterruptFlagRegister(uint8_t value, m_simplestation_state *m_simplestation)
{
    if (value & 0x40)
	{
        clearParameters(m_simplestation);
		updateStatusRegister(m_simplestation);
    }

	if (!interrupt_isempty(m_simplestation))
	{
		interrupt_pop(m_simplestation);
	}
}

uint8_t getStatusRegister(m_simplestation_state *m_simplestation)
{
	return m_simplestation->m_cdrom->status._value;
}

void updateStatusRegister(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->status.parameterFifoEmpty = param_isempty(m_simplestation);
    m_simplestation->m_cdrom->status.parameterFifoFull = !(param_size(m_simplestation) >= 16);
	m_simplestation->m_cdrom->status.responseFifoEmpty = !response_isempty(m_simplestation);
}

uint8_t getInterruptFlagRegister(m_simplestation_state *m_simplestation)
{
    uint8_t flags = 0b11100000;
    
	if (!interrupt_isempty(m_simplestation))
	{
        flags |= interrupt_front(m_simplestation) & 0x7;
    }
    
	return flags;
}

void setState(CDROMState state, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->statusCode.error = false;
    m_simplestation->m_cdrom->statusCode.spindleMotor = true;
    m_simplestation->m_cdrom->statusCode.seekError = false;
    m_simplestation->m_cdrom->statusCode.getIdError = false;

	if (state == Unknown)
	{
    	return;
    }

    uint8_t mask = 1 << state;
	m_simplestation->m_cdrom->statusCode._value |= mask;
}

CDROMModeSectorSize sectorSize(m_simplestation_state *m_simplestation) { return (CDROMModeSectorSize) m_simplestation->m_cdrom->mode._sectorSize; }

CDROMModeSpeed speed(m_simplestation_state *m_simplestation) { return (CDROMModeSpeed) m_simplestation->m_cdrom->mode._speed; }

/* CDROM Commands */
/* 0x01 */
void operationGetstat(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] GetStat" NORMAL "\n");

	if (m_simplestation->m_cdrom_in)
		m_simplestation->m_cdrom->statusCode.shellOpen = 0;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
	interrupt_push(0x03, m_simplestation);
}

/* 0x02 */
void operationSetloc(m_simplestation_state *m_simplestation)
{
    uint8_t second = BCD_DECODE(m_cdrom_parameter_fifo_pop(m_simplestation));
    uint8_t sector = BCD_DECODE(m_cdrom_parameter_fifo_pop(m_simplestation));
	uint8_t minute = BCD_DECODE(m_cdrom_parameter_fifo_pop(m_simplestation));

    m_simplestation->m_cdrom->m_seek_sector = (minute * SecondsPerMinute * SectorsPerSecond) + (second * SectorsPerSecond) + sector;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);

	printf(BOLD MAGENTA "[CDROM] SetLoc (%d, %d, %d | LBA: %d)" NORMAL "\n", minute, second, sector, m_simplestation->m_cdrom->m_seek_sector);
}

/* 0x0E */
void operationSetMode(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] SetMode" NORMAL "\n");

	uint8_t value = m_cdrom_parameter_fifo_pop(m_simplestation);
    m_simplestation->m_cdrom->mode._value = value;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);
}

/* 0x15 */
void operationSeekL(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] SeekL" NORMAL "\n");
	m_simplestation->m_cdrom->m_read_sector = m_simplestation->m_cdrom->m_seek_sector;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);

    setState(Seeking, m_simplestation);

    m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x02, m_simplestation);
}

/* 0x19 */
void operationTest(m_simplestation_state *m_simplestation)
{
	uint8_t subfunction = param_front(m_simplestation);

    switch (subfunction)
	{
        case 0x20:
			printf(BOLD MAGENTA "[CDROM] INT3(yy,mm,dd,ver)" NORMAL "\n");
            m_cdrom_response_fifo_push(0x94, m_simplestation);
            m_cdrom_response_fifo_push(0x09, m_simplestation);
            m_cdrom_response_fifo_push(0x19, m_simplestation);
            m_cdrom_response_fifo_push(0xC0, m_simplestation);
			interrupt_push(0x03, m_simplestation);
            break;
        
        default:
            printf(BOLD RED "[CDROM] test: Unhandled CDROM TEST Sub-Command 0x%X" NORMAL "\n", subfunction);
			m_simplestation_exit(m_simplestation, 1);
			break;
    }
}

/* 0x1A */
void operationGetID(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] GetId" NORMAL "\n");
	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    m_cdrom_response_fifo_push(0x00, m_simplestation);
    m_cdrom_response_fifo_push(0x20, m_simplestation);
    m_cdrom_response_fifo_push(0x00, m_simplestation);
    m_cdrom_response_fifo_push('S', m_simplestation);
    m_cdrom_response_fifo_push('C', m_simplestation);
    m_cdrom_response_fifo_push('E', m_simplestation);
    m_cdrom_response_fifo_push('A', m_simplestation);
    interrupt_push(0x02, m_simplestation);
}

/* CDROM Commands */

void execute(uint8_t command, m_simplestation_state *m_simplestation)
{
	clearInterrupts(m_simplestation);
	clearResponses(m_simplestation);

	switch (command)
	{
		case 0x01:
			operationGetstat(m_simplestation);
			break;

		case 0x02:
			operationSetloc(m_simplestation);
			break;

		case 0x0E:
			operationSetMode(m_simplestation);
			break;

		case 0x15:
			operationSeekL(m_simplestation);
			break;

        case 0x19:
            operationTest(m_simplestation);
            break;
		
		case 0x1A:
			operationGetID(m_simplestation);
			break;

        default:
            printf(BOLD RED "[CDROM] execute: Unhandled CDROM Command 0x%X" NORMAL "\n", command);
			m_simplestation_exit(m_simplestation, 1);
			break;
    }

    clearParameters(m_simplestation);
    updateStatusRegister(m_simplestation);
}

/* Tick */
void cdrom_tick(m_simplestation_state *m_simplestation)
{
    if (!interrupt_isempty(m_simplestation))
	{
        if ((m_simplestation->m_cdrom->interrupt.enable & 0x7) & (interrupt_front(m_simplestation) & 0x7)) {
#ifdef CDROM_DEBUG
			printf("[CDROM] tick: Requesting interrupt...\n");
#endif
            m_interrupts_request(CDROM, m_simplestation);
        }
    }
}

/* Cdrom RW Functions */
uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation)
{
    uint32_t m_value = 0;

	// Available CDROM Ports range from 0-3 
    switch (m_offset)
    {
		// Status Register
		case 0:
			m_value = getStatusRegister(m_simplestation);
			break;

		case 1:
			switch (m_simplestation->m_cdrom->status.index)
			{
                case 1:
					m_value = m_cdrom_response_fifo_pop(m_simplestation);
					break;

				default:
					printf(RED "[CDROM] read: Unhandled Offset 1 CDROM read (Index: %d)\n" NORMAL,
							m_simplestation->m_cdrom->status.index);
					m_simplestation_exit(m_simplestation, 1);
					break;
			}
			break;

		case 3:
			switch (m_simplestation->m_cdrom->status.index)
			{
                case 1:
					m_value = getInterruptFlagRegister(m_simplestation);
					break;

				default:
					printf(RED "[CDROM] read: Unhandled Offset 3 CDROM read (Index: %d)\n" NORMAL,
							m_simplestation->m_cdrom->status.index);
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

void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    switch (m_offset)
    {
		// Index/Status Register
		case 0:
			setStatusRegister(m_value & 0xFF, m_simplestation);
			break;

		case 1:
			execute(m_value, m_simplestation);
			break;

		// Status's Index-determined write
		case 2:
			switch (m_simplestation->m_cdrom->status.index)
			{
				case 0:
					m_cdrom_parameter_fifo_push(m_value, m_simplestation);
					break;

				// Interrupt Enable Register Write
				case 1:
					setInterruptRegister(m_value, m_simplestation);
					break;

				default:
					printf(RED "[CDROM] write: Unhandled Offset 2 CDROM Write (Index: %d, Value: 0x%08X)\n" NORMAL,
							m_simplestation->m_cdrom->status.index, m_value);
					m_simplestation_exit(m_simplestation, 1);
            		break;
			}
			break;

		// Status's Index-determined write
		case 3:
			switch (m_simplestation->m_cdrom->status.index)
			{
				// Interrupt Flag Register Write
				case 1:
					setInterruptFlagRegister(m_value, m_simplestation);
					break;

				default:
					printf(RED "[CDROM] write: Unhandled Offset 3 CDROM Write (Index: %d, Value: 0x%08X)\n" NORMAL,
							m_simplestation->m_cdrom->status.index, m_value);
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
