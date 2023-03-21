#include <cdrom/cdrom.h>
#include <cdrom/parameter_fifo.h>
#include <cdrom/response_fifo.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

FILE *cd = NULL;

const uint32_t SecondsPerMinute = 60;
const uint32_t SectorsPerSecond = 75;
const uint32_t SystemClocksPerCDROMInt1SingleSpeed=33868800/ 75;;
const uint32_t SystemClocksPerCDROMInt1DoubleSpeed=33868800/ 100;

bool rbuf_dirty = false;

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
			m_simplestation->m_cdrom->statusCode.shellOpen = 0;
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
	memset(&m_simplestation->m_cdrom->currentSector, 0, sizeof(m_simplestation->m_cdrom->currentSector));
	memset(m_simplestation->m_cdrom->readBuffer, 0, sizeof(m_simplestation->m_cdrom->readBuffer));
	m_simplestation->m_cdrom->readBufferIndex = 0;
	m_simplestation->m_cdrom->interruptFlag._value = 0;
	m_simplestation->m_cdrom->internalState = IdleState;
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
    }

	m_simplestation->m_cdrom->interruptFlag._value  &= ~(value & 0x1F);

	if (!interrupt_isempty(m_simplestation))
	{
		m_simplestation->m_cdrom->interruptFlag._value |= interrupt_front(m_simplestation);
		interrupt_pop(m_simplestation);
	}
}

uint8_t getStatusRegister(m_simplestation_state *m_simplestation)
{
	updateStatusRegister(m_simplestation);
	return m_simplestation->m_cdrom->status._value;
}

void updateStatusRegister(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->status.parameterFifoEmpty = param_isempty(m_simplestation);
    m_simplestation->m_cdrom->status.parameterFifoFull = !(param_size(m_simplestation) >= 16);
	m_simplestation->m_cdrom->status.responseFifoEmpty = !response_isempty(m_simplestation);
	m_simplestation->m_cdrom->status.dataFifoEmpty = rbuf_dirty ? 1 : 0;
}

uint8_t getInterruptFlagRegister(m_simplestation_state *m_simplestation)
{
    uint8_t flags = 0xe0 | m_simplestation->m_cdrom->interruptFlag._value;
    
	return flags;
}

void setState(CDROMState state, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->statusCode.error = false;
    m_simplestation->m_cdrom->statusCode.spindleMotor = true;
    m_simplestation->m_cdrom->statusCode.seekError = false;
    m_simplestation->m_cdrom->statusCode.getIdError = false;
    m_simplestation->m_cdrom->statusCode._value &= ~(0xE0);

	if (state == Unknown)
	{
    	return;
    }

	m_simplestation->m_cdrom->statusCode._value |= (1 << state);
}

CDROMModeSectorSize sectorSize(m_simplestation_state *m_simplestation) { return (CDROMModeSectorSize) m_simplestation->m_cdrom->mode._sectorSize; }

CDROMModeSpeed speed(m_simplestation_state *m_simplestation) { return (CDROMModeSpeed) m_simplestation->m_cdrom->mode._speed; }

CDSector readSector(uint32_t location, m_simplestation_state *m_simplestation)
{
	printf("[CDROM] Reading from LBA %d\n", location);

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
    if (!rbuf_dirty)
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

size_t snum = 0;

void setRequestRegister(uint8_t value, m_simplestation_state *m_simplestation)
{
    if (value & 0x80) {
		//rbuf_dirty = false;

		if (isReadBufferEmpty(m_simplestation)) {
			memset(m_simplestation->m_cdrom->readBuffer, 0, sizeof(m_simplestation->m_cdrom->readBuffer));
		
            CDROMModeSectorSize sectorSize_ = sectorSize(m_simplestation);
            if (sectorSize_ == DataOnly800h)
			{
				memcpy(m_simplestation->m_cdrom->readBuffer, &m_simplestation->m_cdrom->currentSector.data, 0x800);

				rbuf_dirty = true;

                //copy(&currentSector.data[0], &currentSector.data[0x800], back_inserter(readBuffer));
            } else { // WholeSector924h
                //copy(&currentSector.header[0], &currentSector.ECC[276], back_inserter(readBuffer));
				printf(BOLD RED "[CDROM] setRequestRegister: Unimplemented whole sector copy!" NORMAL "\n");
				m_simplestation_exit(m_simplestation, 1);
            }
            m_simplestation->m_cdrom->readBufferIndex = 0;
            
        }
	} else {
		rbuf_dirty = false;
		m_simplestation->m_cdrom->readBufferIndex = 0;
    }
}

uint8_t loadByteFromReadBuffer(m_simplestation_state *m_simplestation)
{
    if (!rbuf_dirty)
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

    return value;
}

uint32_t loadWordFromReadBuffer(m_simplestation_state *m_simplestation)
{
	uint8_t word[] = { loadByteFromReadBuffer(m_simplestation), loadByteFromReadBuffer(m_simplestation), loadByteFromReadBuffer(m_simplestation), loadByteFromReadBuffer(m_simplestation) };

    return *(uint32_t *)word;
}

/* CDROM Commands */
/* 0x01 */
void operationGetstat(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] GetStat (PC: 0x%X)" NORMAL "\n", PC);

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
	interrupt_push(0x03, m_simplestation);
}

/* 0x02 */
void operationSetloc(m_simplestation_state *m_simplestation)
{
    uint8_t minute = BCD_DECODE(param_front(m_simplestation));
	m_cdrom_parameter_fifo_pop(m_simplestation);
    uint8_t second = BCD_DECODE(param_front(m_simplestation));
	m_cdrom_parameter_fifo_pop(m_simplestation);
	uint8_t sector = BCD_DECODE(param_front(m_simplestation));
	m_cdrom_parameter_fifo_pop(m_simplestation);

    m_simplestation->m_cdrom->m_seek_sector = (minute * SecondsPerMinute * SectorsPerSecond) + (second * SectorsPerSecond) + sector;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x02, m_simplestation);

	printf(BOLD MAGENTA "[CDROM] SetLoc (%d, %d, %d | LBA: %d) (PC: 0x%0X)" NORMAL "\n", minute, second, sector, m_simplestation->m_cdrom->m_seek_sector, PC);
}

/* 0x06 */
void operationReadN(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] ReadN (PC: 0x%X)" NORMAL "\n", PC);

	m_simplestation->m_cdrom->m_read_sector = m_simplestation->m_cdrom->m_seek_sector;

    setState(Reading, m_simplestation);

	m_simplestation->m_cdrom->internalState = ReadingState;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);
}

/* 0x09 */
void operationPause(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] Pause (PC: 0x%X)" NORMAL "\n", PC);

	setState(Unknown, m_simplestation);
	
	m_simplestation->m_cdrom->internalState = IdleState;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);
	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x02, m_simplestation);
}

extern bool debugcpu;

/* 0x0A */
void operationInit(m_simplestation_state *m_simplestation)
{
	debugcpu = true;
	m_simplestation->m_cdrom->statusCode.shellOpen = 0;

	printf(BOLD MAGENTA "[CDROM] Init (PC: 0x%X)" NORMAL "\n", PC);
	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x02, m_simplestation);
}

/* 0x0C */
void operationDemute(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] Demute" NORMAL "\n");

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);
}

/* 0x0E */
void operationSetMode(m_simplestation_state *m_simplestation)
{
	uint8_t value = param_front(m_simplestation);

	printf(BOLD MAGENTA "[CDROM] SetMode (0x%X) (PC: 0x%X)" NORMAL "\n", value, PC);

	m_cdrom_parameter_fifo_pop(m_simplestation);
    m_simplestation->m_cdrom->mode._value = value;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);
}

/* 0x15 */
void operationSeekL(m_simplestation_state *m_simplestation)
{
	printf(BOLD MAGENTA "[CDROM] SeekL (PC: 0x%X)" NORMAL "\n", PC);
	m_simplestation->m_cdrom->m_read_sector = m_simplestation->m_cdrom->m_seek_sector;

	m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x03, m_simplestation);

    setState(Seeking, m_simplestation);
	m_simplestation->m_cdrom->internalState = SeekingState;

    m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);
    interrupt_push(0x02, m_simplestation);
}

/* 0x19 */
void operationTest(m_simplestation_state *m_simplestation)
{
	uint8_t subfunction = param_front(m_simplestation);
	m_cdrom_parameter_fifo_pop(m_simplestation);

    switch (subfunction)
	{
        case 0x20:
			printf(BOLD MAGENTA "[CDROM] INT3(yy,mm,dd,ver) (PC: 0x%X)" NORMAL "\n", PC);
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
	printf(BOLD MAGENTA "[CDROM] GetId (PC: 0x%X)" NORMAL "\n", PC);
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
	
	m_simplestation->m_cdrom->status.transmissionBusy = true;

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

		case 0x09:
			operationPause(m_simplestation);
			break;

		case 0x0A:
			operationInit(m_simplestation);
			break;

		case 0x0C:
			operationDemute(m_simplestation);
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
			interrupt_push(0x05, m_simplestation);
			m_cdrom_response_fifo_push(0x11, m_simplestation);
			m_cdrom_response_fifo_push(0x10, m_simplestation);
			//m_simplestation_exit(m_simplestation, 1);
			break;
    }
}

/* Tick */
void cdrom_tick(uint32_t cycles, m_simplestation_state *m_simplestation)
{
	m_simplestation->m_cdrom->m_count += cycles;

    if (!interrupt_isempty(m_simplestation) && m_simplestation->m_cdrom->interruptFlag._value == 0)
	{
		m_simplestation->m_cdrom->interruptFlag._value |= interrupt_front(m_simplestation);
		interrupt_pop(m_simplestation);
	}

	if (m_simplestation->m_cdrom->interrupt._value & m_simplestation->m_cdrom->interruptFlag._value)
	{
        m_simplestation->m_cdrom->status.transmissionBusy = true;

#ifdef CDROM_DEBUG
		printf("[CDROM] tick: Requesting interrupt...\n");
#endif
        m_interrupts_request(CDROM, m_simplestation);
        return;
    }

	switch (m_simplestation->m_cdrom->internalState) {
        case IdleState: {
            if (!interrupt_isempty(m_simplestation)) {
                return;
            }
            m_simplestation->m_cdrom->m_count = 0;
            break;
        }
        case SeekingState: {
            // TODO: This is what works but not sure how far this is going to fly
            if (m_simplestation->m_cdrom->m_count < 100000 || !interrupt_isempty(m_simplestation)) {
                return;
            }
            m_simplestation->m_cdrom->m_count = 0;
            break;
        }
        case ReadingState: {
            if (m_simplestation->m_cdrom->m_count < SystemClocksPerCDROMInt1DoubleSpeed) {
                return;
            }
            m_simplestation->m_cdrom->currentSector = readSector(m_simplestation->m_cdrom->m_read_sector, m_simplestation);
            if (!interrupt_isempty(m_simplestation)) {
                return;
            }
			interrupt_push(0x01, m_simplestation);
			m_cdrom_response_fifo_push(m_simplestation->m_cdrom->statusCode._value, m_simplestation);

			m_simplestation->m_cdrom->m_count = 0;

			m_simplestation->m_cdrom->m_read_sector++;
		}
		case ReadingTableOfContentsState: {
            printf("[CDROM] Unhandled reading TOC\n");
			//m_simplestation_exit(m_simplestation, 1);
            break;
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
					m_value = response_front(m_simplestation);
					m_cdrom_response_fifo_pop(m_simplestation);
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
			switch (m_simplestation->m_cdrom->status.index)
			{
				case 0:
					execute(m_value, m_simplestation);
					break;

				default:
					printf(RED "[CDROM] write: Unhandled Offset 0 CDROM Write (Index: %d, Value: 0x%08X)\n" NORMAL,
							m_simplestation->m_cdrom->status.index, m_value);
					//m_simplestation_exit(m_simplestation, 1);
            		break;
			}
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

				case 2:
					break;

				case 3:
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

				case 2:
					break;

				case 3:
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
