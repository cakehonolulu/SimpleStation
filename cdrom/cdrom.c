#include <cdrom/cdrom.h>
#include <cdrom/parameter_fifo.h>
#include <cdrom/response_fifo.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

FILE *cd = NULL;

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

		if (m_simplestation->m_cdrom_in)
			cd = fopen(m_simplestation->cd_name, "rb");
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
		fclose(cd);
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
	m_simplestation->m_cdrom->m_count = 0;
	memset(m_simplestation->m_cdrom->currentSector, 0, sizeof(m_simplestation->m_cdrom->currentSector));
	memset(m_simplestation->m_cdrom->readBuffer, 0, sizeof(m_simplestation->m_cdrom->readBuffer));
	m_simplestation->m_cdrom->readBufferIndex = 0;
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

CDSector readSector(uint32_t location, m_simplestation_state *m_simplestation)
{
    // TODO: there is a two seconds pre-gap at the first index of the single-track data CDs *only*
    location -= (2 * SectorsPerSecond);

    CDSector sector;
    fseek(cd, location * sizeof(CDSector), SEEK_SET);

	fread(&sector, sizeof(sector), 1, cd);

    return sector;
}

uint8_t getInterruptRegister(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->interrupt.enable;
}

bool isReadBufferEmpty(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom->readBufferIndex == 0)
	{
        return true;
    }

    CDROMModeSectorSize sectorSize_ = sectorSize(m_simplestation);

    if (sectorSize_ == DataOnly800h) {
        return m_simplestation->m_cdrom->readBufferIndex >= 0x800;
    } else { // WholeSector924h
        return m_simplestation->m_cdrom->readBufferIndex >= 0x924;
    }
}

void setRequestRegister(uint8_t value, m_simplestation_state *m_simplestation)
{
    if (value & 0x80) {
        if (isReadBufferEmpty(m_simplestation)) {
            CDROMModeSectorSize sectorSize_ = sectorSize(m_simplestation);
            if (sectorSize_ == DataOnly800h)
			{
				memcpy(m_simplestation->m_cdrom->readBuffer, &m_simplestation->m_cdrom->currentSector.data[0], 0x800);

                //copy(&currentSector.data[0], &currentSector.data[0x800], back_inserter(readBuffer));
            } else { // WholeSector924h
                //copy(&currentSector.header[0], &currentSector.ECC[276], back_inserter(readBuffer));
				printf(BOLD RED "[CDROM] setRequestRegister: Unimplemented whole sector copy!" NORMAL "\n");
				m_simplestation_exit(m_simplestation, 1);
            }
            m_simplestation->m_cdrom->readBufferIndex = 0;
            m_simplestation->m_cdrom->status.dataFifoEmpty = 1;
        } else {
			memset(m_simplestation->m_cdrom->readBuffer, 0, sizeof(m_simplestation->m_cdrom->readBuffer));
            m_simplestation->m_cdrom->readBufferIndex = 0;
            m_simplestation->m_cdrom->status.dataFifoEmpty = 0;
        }
    }
}

uint8_t loadByteFromReadBuffer(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom->readBufferIndex == 0)
	{
        return 0;
    }

    /*
		The PSX hardware allows to read 800h-byte or 924h-byte sectors, indexed as [000h..7FFh] or
		[000h..923h], when trying to read further bytes, then the PSX will repeat the byte at
		index [800h-8] or [924h-4] as padding value.
    */
    CDROMModeSectorSize sectorSize_ = sectorSize(m_simplestation);
    if (sectorSize_ == DataOnly800h && m_simplestation->m_cdrom->readBufferIndex >= 0x800) {
        return m_simplestation->m_cdrom->readBuffer[0x800 - 0x8];
    } else if (sectorSize_ == WholeSector924h && m_simplestation->m_cdrom->readBufferIndex >= 0x924) {
        return m_simplestation->m_cdrom->readBuffer[0x924 - 0x4];
    }

    uint8_t value = m_simplestation->m_cdrom->readBuffer[m_simplestation->m_cdrom->readBufferIndex];
    m_simplestation->m_cdrom->readBufferIndex++;
    if (isReadBufferEmpty(m_simplestation)) {
        m_simplestation->m_cdrom->status.dataFifoEmpty = 0;
    }

    return value;
}

uint32_t loadWordFromReadBuffer(m_simplestation_state *m_simplestation)
{
    uint32_t value = 0;
    for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
        uint8_t byte = loadByteFromReadBuffer(m_simplestation);
        value |= (((uint32_t)byte) << (i * 8));
    }
    return value;
}

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

/* 0x06 */
void operationReadN(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] ReadN" NORMAL "\n");

	m_simplestation->m_cdrom->m_read_sector = m_simplestation->m_cdrom->m_seek_sector;

    setState(Reading, m_simplestation);

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);
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

		case 0x06:
			operationReadN(m_simplestation);
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

	m_simplestation->m_cdrom->m_count++;

    if ((m_simplestation->m_cdrom->statusCode.play || m_simplestation->m_cdrom->statusCode.read) && m_simplestation->m_cdrom->m_count >= SystemClocksPerCDROMInt1DoubleSpeed)
	{
    	interrupt_push(0x01, m_simplestation);
		m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);

        m_simplestation->m_cdrom->m_count = 0;

        m_simplestation->m_cdrom->currentSector = readSector(m_simplestation->m_cdrom->m_read_sector, m_simplestation);
        m_simplestation->m_cdrom->m_read_sector++;
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
				case 0:
					m_value = getInterruptRegister(m_simplestation);
					break;

                case 1:
					m_value = getInterruptFlagRegister(m_simplestation);
					break;
				
				case 2:
					m_value = getInterruptRegister(m_simplestation);
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
				case 0:
					setRequestRegister(m_value, m_simplestation);
					break;

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
