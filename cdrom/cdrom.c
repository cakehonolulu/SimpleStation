#include <cdrom/cdrom.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

FILE *cd = NULL;

size_t file_size = 0;

SeekParam seekParam;

const int SECTOR_SIZE = 2352;
const int READ_SIZE   = 0x818;

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

void INT2(m_simplestation_state *m_simplestation)
{
    printf("[CD-ROM    ] INT2\n");

    m_simplestation->m_cdrom->iFlags |= (uint8_t) 2;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);
}

void INT3(m_simplestation_state *m_simplestation)
{
    printf("[CD-ROM    ] INT3\n");

    m_simplestation->m_cdrom->iFlags |= (uint8_t) 3;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);
}

/* 0x01 */
void cmdGetStat(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

    printf("[CD-ROM    ] Get Stat\n");

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

/* 0x19 */
void cmdTest(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	uint8_t subfunction = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);

	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

	switch (subfunction)
	{
		case 0x20:
			printf(BOLD MAGENTA "[CD-ROM    ] INT3(yy,mm,dd,ver)" NORMAL "\n");
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x94);
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x09);
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0x19);
			cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 0xC0);

			primary_event.time = m_simplestation->time + INT3_TIME;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			break;

		default:
			printf("[CD-ROM    ] Unhandled TEST Sub-command: 0x%x\n", subfunction);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}
/* 0x1A */
void cmdGetID(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");
    printf("[CD-ROM    ] Get ID\n");

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

    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'C');
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'A');
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'K');
    cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, 'E');

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

		case Test:
			cmdTest(m_simplestation);
			break;

		case GetID:
			cmdGetID(m_simplestation);
			break;

        default:
            printf("[CD-ROM    ] Unhandled command 0x%02X\n", m_simplestation->m_cdrom->cmd);

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
			//printf("[CD-ROM    ] 8-bit read @ STATUS\n");

            m_value |= cqueue_i_empty(&m_simplestation->m_cdrom->paramFIFO) << 3;        // Parameter FIFO empty
            m_value |= (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO) != 16) << 4; // Parameter FIFO not full
            m_value |= !cqueue_i_empty(&m_simplestation->m_cdrom->responseFIFO) << 5;    // Response FIFO not empty

			break;

		case 1:
			//printf("[CD-ROM    ] 8-bit read @ RESPONSE\n");

            m_value = (uint32_t) *cqueue_i_front(&m_simplestation->m_cdrom->responseFIFO);

			cqueue_i_pop(&m_simplestation->m_cdrom->responseFIFO);

			break;

		case 3:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 1:
					//printf("[CD-ROM    ] 8-bit read @ IF\n");
                    m_value = m_simplestation->m_cdrom->iFlags;
					break;

				default:
					printf("[CD-ROM    ] Unhandled 8-bit read @ 0x%08X.%u\n", m_offset, m_simplestation->m_cdrom->index_);
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
			//printf("[CD-ROM    ] 8-bit write @ INDEX = 0x%02X\n", m_value);

            m_simplestation->m_cdrom->index_ = m_value & 3;
			break;

		case 1:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
					//printf("[CD-ROM    ] 8-bit write @ CMD = 0x%02X\n", m_value);

                    doCmd(m_value, m_simplestation);
					break;

				default:
					printf("[CD-ROM    ] Unhandled 8-bit write @ offset %u = 0x%02X\n", m_offset, m_value);
					break;
			}
			break;
		
		case 2:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
					//printf("[CD-ROM    ] 8-bit write @ PARAM = 0x%02X\n", m_value);

                    //assert(paramFIFO.size() < 16);

					cqueue_i_push(&m_simplestation->m_cdrom->paramFIFO, m_value);
					break;

				case 1:
					//printf("[CD-ROM    ] 8-bit write @ IE = 0x%02X\n", m_value);

                    m_simplestation->m_cdrom->iEnable = m_value & 0x1F;
					break;

				default:
					printf("[CD-ROM    ] Unhandled 8-bit write @ %u = 0x%02X\n", m_offset, m_value);

					m_simplestation_exit(m_simplestation, 1);
					break;
			}
			break;

		case 3:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 1:
                    //printf("[CD-ROM    ] 8-bit write @ IF = 0x%02X\n", m_value);

                    m_simplestation->m_cdrom->iFlags &= (~m_value & 0x1F);
                    break;

				default:
					printf("[CD-ROM    ] Unhandled 8-bit write @ %u = 0x%02X\n", m_offset, m_value);
					break;
			}
			break;

        default:
            printf(RED "[CDROM] write: Unhandled CDROM Write (Offset: %d, Value: 0x%08X)\n" NORMAL, m_offset, m_value);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }
}
