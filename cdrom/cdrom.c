#include <cdrom/cdrom.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

FILE *cd = NULL;

size_t file_size = 0;

SeekParam seekParam;

uint8_t readBuf[SECTOR_SIZE];
int readIdx = 0;

uint8_t toChar(uint8_t bcd)
{
	return (bcd / 16) * 10 + (bcd % 16);
}

uint8_t m_cdrom_init(m_simplestation_state *m_simplestation)
{
    uint8_t m_result = 0;

    m_simplestation->m_cdrom = (m_psx_cdrom_t *) malloc(sizeof(m_psx_cdrom_t));

	m_simplestation->m_cdrom->paramFIFO = cqueue_i_init();
	
	m_simplestation->m_cdrom->responseFIFO = cqueue_i_init();


    if (m_simplestation->m_cdrom)
    {
        m_simplestation->m_cdrom_state = ON;
        memset(m_simplestation->m_cdrom, 0, sizeof(m_psx_cdrom_t));
		m_cdrom_setup(m_simplestation);

		if (m_simplestation->m_cdrom_in)
		{
			cd = fopen(m_simplestation->cd_name, "rb");
			fseek(cd, 0L, SEEK_END);
			file_size = ftell(cd);

			fseek(cd, 0L, SEEK_SET);

		}

    }
    else
    {
        m_result = 1;
    }

    return m_result;
}

void m_cdrom_setup(m_simplestation_state *m_simplestation)
{

}

void m_cdrom_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom)
    {
		fclose(cd);
        free(m_simplestation->m_cdrom);
    }
}

void readSector(m_simplestation_state *m_simplestation) {
    /* Calculate seek target (in sectors) */
    uint32_t mm   = toChar(seekParam.mins) * 60 * 75; // 1min = 60sec
    uint32_t ss   = toChar(seekParam.secs) * 75; // 1min = 75 sectors
    uint32_t sect = toChar(seekParam.sector);

    uint32_t seekTarget = mm + ss + sect - 150; // Starts at 2s, subtract 150 sectors to get start

    printf("[CDROM    ] Seeking to [%02X:%02X:%02X] (LBA: %d)\n", seekParam.mins, seekParam.secs, seekParam.sector, seekTarget);

	fseek(cd, seekTarget * SECTOR_SIZE, SEEK_SET);

	fread(&readBuf, SECTOR_SIZE, sizeof(uint8_t), cd);

    readIdx = (m_simplestation->m_cdrom->mode & (uint8_t)(FullSector)) ? 12 : 24;

    seekParam.sector++;

    /* Increment BCD values */
    if ((seekParam.sector & 0xF) == 10) { seekParam.sector += 10; seekParam.sector &= 0xF0; }

    if (seekParam.sector == 0x75) { seekParam.secs++; seekParam.sector = 0; }

    if ((seekParam.secs & 0xF) == 10) { seekParam.secs += 10; seekParam.secs &= 0xF0; }

    if (seekParam.secs == 0x60) { seekParam.mins++; seekParam.secs = 0; }

    if ((seekParam.mins & 0xF) == 10) { seekParam.mins += 10; seekParam.mins &= 0xF0; }

    printf("[CDROM    ] Next seek to [%02X:%02X:%02X]\n", seekParam.mins, seekParam.secs, seekParam.sector);
}

uint32_t getData32(m_simplestation_state *m_simplestation) {
    //assert(readIdx < SECTOR_SIZE);

    uint32_t data;

    memcpy(&data, &readBuf[readIdx], 4);

    readIdx += 4;

    return data;
}

void INT1(m_simplestation_state *m_simplestation)
{
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

    //printf("[CDROM    ] INT2\n");

    m_simplestation->m_cdrom->iFlags |= (uint8_t) 1;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);

	readSector(m_simplestation);

	primary_event.time = m_simplestation->time + (250000 + 250000 * !(m_simplestation->m_cdrom->mode & (uint8_t)(Speed)));
	primary_event.func = &INT1;
	scheduler_push(primary_event, m_simplestation);
}

void INT2(m_simplestation_state *m_simplestation)
{
    //printf("[CDROM    ] INT2\n");

    m_simplestation->m_cdrom->iFlags |= (uint8_t) 2;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);
}

void INT3(m_simplestation_state *m_simplestation)
{
    //printf("[CDROM    ] INT3\n");

    m_simplestation->m_cdrom->iFlags |= (uint8_t) 3;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);
}

/* 0x01 */
void cmdGetStat(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

    printf(BOLD MAGENTA "[CDROM    ] GetStat" NORMAL "\n");

    // Send status
	if (m_simplestation->m_cdrom_in)
	{
		cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    	// Clear shell open flag
    	m_simplestation->m_cdrom->stat &= ~(uint8_t)(ShellOpen);
	}
	else
	{
		cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0b00010000);
	}

    // Send INT3
	primary_event.time = m_simplestation->time + 20000;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x02 */
void cmdSetLoc(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM    ] SetLoc " NORMAL "\n");

    // Send status
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    /* Set minutes, seconds, sector */
    seekParam.mins   = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

    seekParam.secs   = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

    seekParam.sector = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);
	
    // Send INT3
	primary_event.time = m_simplestation->time + 30000;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x06 */
void cmdReadN(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM    ] ReadN" NORMAL "\n");

	// Send status
	cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    // Send INT3
	primary_event.time = m_simplestation->time + 30000;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

	m_simplestation->m_cdrom->stat |= (uint8_t)(Read);

	// Send status
	cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    // Send INT2
	primary_event.time = m_simplestation->time + (30000 + 250000 + 250000 * !(m_simplestation->m_cdrom->mode & (uint8_t)(Speed)));
	primary_event.func = &INT1;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x09 */
void cmdPause(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM    ] Pause" NORMAL "\n");

	scheduler_clean(m_simplestation);

	cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    // Send INT3
	primary_event.time = m_simplestation->time + 20000;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

    m_simplestation->m_cdrom->stat &= ~(uint8_t)(Play);
    m_simplestation->m_cdrom->stat &= ~(uint8_t)(Read);

    // Send status
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    // Send INT2
	primary_event.time = m_simplestation->time + 120000;
	primary_event.func = &INT2;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x0E */
void cmdSetMode(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	// Send status
	cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    m_simplestation->m_cdrom->mode = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

	printf(BOLD MAGENTA "[CDROM    ] SetMode (0x%X) " NORMAL "\n", m_simplestation->m_cdrom->mode);

    // Send INT3
	primary_event.time = m_simplestation->time + 30000;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x15 */
void cmdSeekL(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM    ] SeekL" NORMAL "\n");

    // Send status
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    // Send INT3
	primary_event.time = m_simplestation->time + 30000;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

    m_simplestation->m_cdrom->stat |= (uint8_t) (Seek);

    // Send status
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    // Send INT2
	primary_event.time = m_simplestation->time + 120000;
	primary_event.func = &INT2;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x19 */
void cmdTest(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	uint8_t subfunction = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);

	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

	switch (subfunction)
	{
		case 0x20:
			printf(BOLD MAGENTA "[CDROM    ] INT3(yy,mm,dd,ver)" NORMAL "\n");
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x94);
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x09);
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x19);
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0xC0);

			primary_event.time = m_simplestation->time + INT3_TIME;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			break;

		default:
			printf(BOLD RED "[CDROM    ] Unhandled TEST Sub-command: 0x%x" NORMAL "\n", subfunction);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}
/* 0x1A */
void cmdGetID(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");
    printf(BOLD MAGENTA "[CDROM    ] GetID" NORMAL "\n");

    // Send status
	cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat);

    // Send INT3
	primary_event.time = m_simplestation->time + 30000;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

    /* Licensed, Mode2 */
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, m_simplestation->m_cdrom->stat & ~(1 << 3));
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x00);
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x20);
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x00);

    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'S');
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'C');
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'E');
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'A');

    // Send INT2
	primary_event.time = m_simplestation->time + 50000;
	primary_event.func = &INT2;
	scheduler_push(primary_event, m_simplestation);
}

void doCmd(uint8_t m_value, m_simplestation_state *m_simplestation) {
    m_simplestation->m_cdrom->cmd = m_value;

    switch (m_simplestation->m_cdrom->cmd)
	{
		case GetStat:
			cmdGetStat(m_simplestation);
			break;

		case SetLoc:
			cmdSetLoc(m_simplestation);
			break;

		case ReadN:
			cmdReadN(m_simplestation);
			break;

		case Pause:
			cmdPause(m_simplestation);
			break;

		case SetMode:
			cmdSetMode(m_simplestation);
			break;

		case SeekL:
			cmdSeekL(m_simplestation);
			break;

		case Test:
			cmdTest(m_simplestation);
			break;

		case GetID:
			cmdGetID(m_simplestation);
			break;

        default:
            printf(BOLD RED "[CDROM    ] Unhandled command 0x%02X" NORMAL "\n", m_simplestation->m_cdrom->cmd);

            m_simplestation_exit(m_simplestation, 1);
			break;
    }
}

/* Cdrom RW Functions */
uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation)
{
    uint32_t m_value = 0;

	// Available CDROM Ports range from 0-3 
    switch (m_offset)
    {
		case 0:
			//printf("[CDROM    ] 8-bit read @ STATUS\n");

            m_value |= cqueue_i_empty(&m_simplestation->m_cdrom->paramFIFO) << 3;        // Parameter FIFO empty
            m_value |= (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO) != 16) << 4; // Parameter FIFO not full
            m_value |= !cqueue_i_empty(&m_simplestation->m_cdrom->responseFIFO) << 5;    // Response FIFO not empty

			break;

		case 1:
			//printf("[CDROM    ] 8-bit read @ RESPONSE\n");

            m_value = (uint32_t) *cqueue_i_front(&m_simplestation->m_cdrom->responseFIFO);

			cqueue_i_pop(&m_simplestation->m_cdrom->responseFIFO);

			break;

		case 3:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
                    //printf("[CDROM    ] 8-bit read @ IE\n");
                    m_value = m_simplestation->m_cdrom->iEnable;
					break;

				case 1:
					//printf("[CDROM    ] 8-bit read @ IF\n");
                    m_value = m_simplestation->m_cdrom->iFlags;
					break;

				default:
					printf(BOLD RED "[CDROM    ] Unhandled 8-bit read at Offset %d (Index: %d)" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_);
                    m_value = m_simplestation_exit(m_simplestation, 1);
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
		case 0:
			//printf("[CDROM    ] 8-bit write @ INDEX = 0x%02X\n", m_value);

            m_simplestation->m_cdrom->index_ = m_value & 3;
			break;

		case 1:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
					//printf("[CDROM    ] 8-bit write @ CMD = 0x%02X\n", m_value);

                    doCmd(m_value, m_simplestation);
					break;

				default:
					printf(BOLD RED "[CDROM    ] Unhandled 8-bit write at Offset: %u (Index: %d) = 0x%02X" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_, m_value);
					break;
			}
			break;
		
		case 2:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
					//printf("[CDROM    ] 8-bit write @ PARAM = 0x%02X\n", m_value);

                    //assert(paramFIFO.size() < 16);

					cqueue_i_push(&m_simplestation->m_cdrom->paramFIFO, m_value);
					break;

				case 1:
					//printf("[CDROM    ] 8-bit write @ IE = 0x%02X\n", m_value);

                    m_simplestation->m_cdrom->iEnable = m_value & 0x1F;
					break;

				default:
					printf(BOLD RED "[CDROM    ] Unhandled 8-bit write at Offset: %u (Index: %d) = 0x%02X" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_, m_value);

					m_simplestation_exit(m_simplestation, 1);
					break;
			}
			break;

		case 3:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
                    // printf("[CDROM    ] 8-bit write @ REQUEST = 0x%02X\n", m_value);
                    break;

				case 1:
                    //printf("[CDROM    ] 8-bit write @ IF = 0x%02X\n", m_value);

                    m_simplestation->m_cdrom->iFlags &= (~m_value & 0x1F);
                    break;

				default:
					printf(BOLD RED "[CDROM    ] Unhandled 8-bit write at Offset: %u (Index: %d) = 0x%02X" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_, m_value);
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
