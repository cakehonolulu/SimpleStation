#include <cdrom/cdrom.h>
#include <cdrom/parameter_fifo.h>
#include <cdrom/response_fifo.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

uint8_t queued_fifo[16] = { 0 };
bool reading = false;

#define SECTOR_SIZE 0x930
uint8_t SectorBuffer[SECTOR_SIZE];
int bytes_read = 0;
bool form2 = false;
uint8_t drqsts = 0;
FILE *cd = NULL;
bool xa_adpcm = false;
bool WholeSector = false;

#define FORM1_DATA_SIZE 0x800
#define FORM2_DATA_SIZE 0x914

void cd_init(m_simplestation_state *m_simplestation)
{
	cd = fopen(m_simplestation->cd_name, "rb");
}

uint8_t m_cdrom_init(m_simplestation_state *m_simplestation)
{
    uint8_t m_result = 0;

    m_simplestation->m_cdrom = (m_psx_cdrom_t *) malloc(sizeof(m_psx_cdrom_t));

    if (m_simplestation->m_cdrom)
    {
		if (m_simplestation->m_cdrom_in)
		{
			printf(BOLD GREEN "[CDROM] Booting CD..." NORMAL "\n");
			cd_init(m_simplestation);
		}
		else
		{
			printf(BOLD GREEN "[CDROM] CD-less boot..." NORMAL "\n");
		}

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
			m_simplestation->m_cdrom->m_status_register.raw &= ~0b11;
			m_simplestation->m_cdrom->m_status_register.raw |= m_value & 0b11;
			break;

		// Command Register Write
		case 1:
			switch (m_simplestation->m_cdrom->m_status_register.raw & 0b11) {
				case 0:
					m_cdrom_exec_cmd(m_value, m_simplestation);
					break;
				
				case 3: break;

				default:
					printf(RED "[CDROM] read: Unhandled CDROM Write (Offset: %d, Index: %d)\n" NORMAL, m_offset, m_simplestation->m_cdrom->m_status_register.raw & 0b11);
					m_simplestation_exit(m_simplestation, 1);
					break;
				}

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

				case 2:
					break;

				case 3:
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
				case 0:
					break;

				// Interrupt Flag Register Write
				case 1:
					m_simplestation->m_cdrom->m_interrupt_flag_register &= ~m_value;
					break;

				case 2:
					break;

				case 3:
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
			m_value = m_simplestation->m_cdrom->m_status_register.raw | (drqsts << 6) | ((xa_adpcm ? 1 : 0) << 2);
			break;

		// Response FIFO Read
		case 1:
			switch (m_simplestation->m_cdrom->m_status_register.raw & 0b11) {
				case 0:
				case 1:
					m_value = m_cdrom_response_fifo_pop(m_simplestation);
					break;
				default:
					printf(RED "[CDROM] read: Unhandled CDROM Read (Offset: %d, Index: %d)\n" NORMAL, m_offset, m_simplestation->m_cdrom->m_status_register.raw & 0b11);
					m_simplestation_exit(m_simplestation, 1);
					break;
				}
			
			break;

		case 3:
			switch (m_simplestation->m_cdrom->m_status_register.raw & 0b11) {
				case 0:
					m_value = m_simplestation->m_cdrom->m_interrupt_enable_register;
					break;
				case 1:
					m_value = m_simplestation->m_cdrom->m_interrupt_flag_register;
					break;
				default:
					printf(RED "[CDROM] read: Unhandled CDROM Read (Offset: %d, Index: %d)\n" NORMAL, m_offset, m_simplestation->m_cdrom->m_status_register.raw & 0b11);
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

uint32_t seekloc = 0;

bool cancel_int1 = false;

uint8_t get_stat() {
	uint8_t stat = 0b10;	// Pretend that the motor is always on
	if (reading)
		stat |= (1 << 5);
	return stat;
}

void cd_read(uint32_t loc) {
	printf("[CDROM] Reading from LBA %d\n", loc);
	fseek(cd, (loc - 150) * SECTOR_SIZE, SEEK_SET);
	fread(SectorBuffer, sizeof(uint8_t), SECTOR_SIZE, cd);
	form2 = (SectorBuffer[0x12] & 32);
}

uint8_t ReadDataByte(m_simplestation_state *m_simplestation)
{
	if (bytes_read > ((WholeSector ? (SECTOR_SIZE - 0xc) : (form2 ? FORM2_DATA_SIZE : FORM1_DATA_SIZE)) - 1)) {
		printf("[CD] bytes_read overflow\n");
		exit(0);
	}
	if (bytes_read >= ((WholeSector ? (SECTOR_SIZE - 0xc) : (form2 ? FORM2_DATA_SIZE : FORM1_DATA_SIZE)) - 1)) {
		//printf("[All data has been read]\n");
		drqsts = 0;
	}
	return SectorBuffer[(WholeSector ? 0x0c : 0x18) + bytes_read++];
}


void INT1(m_simplestation_state *m_simplestation)
{
	if (!cancel_int1) {
		cd_read(seekloc++);

		bool audio_sector = (SectorBuffer[0x12] & 4);
		bool realtime = (SectorBuffer[0x12] & 64);
		bool form2 = (SectorBuffer[0x12] & 32);

		if (!xa_adpcm || !(audio_sector && realtime)) {
			if (form2) {
				//printf("[CDROM] MODE2/FORM2 sectors are not supported\n");
				//exit(1);
			}
			//printf(BOLD MAGENTA "[CDROM] INT1" NORMAL "\n");
			m_simplestation->m_cdrom->m_interrupt_flag_register &= ~0b111;
			m_simplestation->m_cdrom->m_interrupt_flag_register |= 0b001;

			m_cdrom_response_fifo_push(get_stat(), m_simplestation);

			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;
			bytes_read = 0;
		}
		event_t event;
		event.time = m_simplestation->time + (33868800 / 75);
		event.func = &INT1;
		strcpy(event.subsystem, "CDROM");
		scheduler_push(event, m_simplestation);
	m_interrupts_request(CDROM, m_simplestation);
	}
	else {
		cancel_int1 = false;
	//m_interrupts_request(CDROM, m_simplestation);
	}
}

void INT2(m_simplestation_state *m_simplestation)
{
	//printf(BOLD MAGENTA "[CDROM] INT2" NORMAL "\n");
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
	//printf(BOLD MAGENTA "[CDROM] INT3" NORMAL "\n");
	m_simplestation->m_cdrom->m_interrupt_flag_register &= ~0b111;
	m_simplestation->m_cdrom->m_interrupt_flag_register |= 0b011;
	m_interrupts_request(CDROM, m_simplestation);
}

void m_cdrom_exec_cmd(uint8_t m_cmd, m_simplestation_state *m_simplestation)
{
	event_t primary_event, secondary_event;
	strcpy(primary_event.subsystem, "CDROM");
	strcpy(secondary_event.subsystem, "CDROM");

	switch(m_cmd)
	{
		case CDROM_GETSTAT_CMD:
			uint8_t status = get_stat();
			status &= ~0b11101;

			/*
				HACK:
				
				Returning CDROM STAT w/bit 4 ('ShellOpen') set, enables the
				'no-disk' mode for the shell (Informs the shell that the CDROM 'tray'
				is currently opened == !disk == jump to shell).
			*/
			if (m_simplestation->m_cdrom_in)
			{
				m_cdrom_response_fifo_push(0b00000000, m_simplestation);
			}
			else
			{
				m_cdrom_response_fifo_push(0b00010000, m_simplestation);
			}

			m_simplestation->m_cdrom->m_queued_responses = 1;
			
			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			//printf("Created event CDROM_GETSTAT_CMD...\n");

			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;

			break;

		case CDROM_SETLOC_CMD:
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;
			
			uint8_t ff = m_cdrom_parameter_fifo_pop(m_simplestation);
			uint8_t ss = m_cdrom_parameter_fifo_pop(m_simplestation);
			uint8_t mm = m_cdrom_parameter_fifo_pop(m_simplestation);

			mm = (mm - 6 * (mm >> 4));
			ss = (ss - 6 * (ss >> 4));
			ff = (ff - 6 * (ff >> 4));

			seekloc = ff + (ss * 75) + (mm * 60 * 75);
			printf("[CDROM] SetLoc (%d;%d;%d %d)\n", mm, ss, ff, seekloc);
			m_cdrom_response_fifo_push(get_stat(), m_simplestation);
			
			m_simplestation->m_cdrom->m_queued_responses = 1;
			
			primary_event.time = m_simplestation->time + 18491;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			//printf("Created event CDROM_SETLOC_CMD...\n");


			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;
			break;

		case 0x03:
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;
			
			uint8_t ff2 = m_cdrom_parameter_fifo_pop(m_simplestation);
			uint8_t ss2 = m_cdrom_parameter_fifo_pop(m_simplestation);
			uint8_t mm2 = m_cdrom_parameter_fifo_pop(m_simplestation);

			mm2 = (mm2 - 6 * (mm2 >> 4));
			ss2 = (ss2 - 6 * (ss2 >> 4));
			ff2 = (ff2 - 6 * (ff2 >> 4));

			printf("[CDROM] Play");
			m_cdrom_response_fifo_push(get_stat(), m_simplestation);
			
			m_simplestation->m_cdrom->m_queued_responses = 1;

			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;
			break;

		case CDROM_READN_CMD:
			if (reading) cancel_int1 = true;
			reading = true;
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;	// Set parameter fifo empty bit
			printf("[CDROM] ReadN\n");
			bytes_read = 0;

			m_cdrom_response_fifo_push(get_stat(), m_simplestation);

			queued_fifo[0] = 0x22;

			m_simplestation->m_cdrom->m_queued_responses = 1;
			
			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);

			secondary_event.time = m_simplestation->time + (33868800 / 75);
			secondary_event.func = &INT1;
			scheduler_push(secondary_event, m_simplestation);

			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;	// Set response fifo empty bit (means it's full)
			break;

		case CDROM_PAUSE_CMD:
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;	// Set parameter fifo empty bit
			printf("%x [CDROM] Pause\n", PC);
			if(reading) cancel_int1 = true;
			m_cdrom_response_fifo_push(get_stat(), m_simplestation);

			m_simplestation->m_cdrom->m_queued_responses = 1;
			
			reading = false;	// The first response's status code still has bit 5 set
			queued_fifo[0] = get_stat();
			
			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);

			secondary_event.time = m_simplestation->time + 50401 + 1097107;
			secondary_event.func = &INT2;
			scheduler_push(secondary_event, m_simplestation);

			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;	// Set response fifo empty bit (means it's full)
			break;

		case 0x0A:
			printf("[CDROM] Init\n");
			m_cdrom_response_fifo_push(get_stat(), m_simplestation);;

			queued_fifo[0] = 0b00000010;
			
			m_simplestation->m_cdrom->m_queued_responses = 1;
			
			primary_event.time = m_simplestation->time + 30000;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);

			secondary_event.time = m_simplestation->time + 40000;
			secondary_event.func = &INT2;
			scheduler_push(secondary_event, m_simplestation);


			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;

			break;

		case 0x0C:
			printf("[CDROM] Demute\n");
			m_cdrom_response_fifo_push(get_stat(), m_simplestation);;
			m_simplestation->m_cdrom->m_queued_responses = 1;

			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);

			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;

			break;

		case CDROM_SETMODE_CMD:
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;
			uint8_t param = m_cdrom_parameter_fifo_pop(m_simplestation);
			printf("[CDROM] SETMODE (0x%X)\n", param);
			int DoubleSpeed = (param & 0b10000000) >> 7;
			xa_adpcm = (param & 0b01000000) >> 6;
			int WholeSector1 = (param & 0b00100000) >> 5;
			int CDDA = param & 1;
			if (DoubleSpeed) //printf("DoubleSpeed\n");
			if (xa_adpcm) //printf("XA-ADPCM\n");
			if (WholeSector1) {
				printf("WholeSector\n");
			}
			WholeSector = WholeSector1;
			if (CDDA) { //printf("CDDA\n"); 
				//exit(1); 
			}

			m_cdrom_response_fifo_push(get_stat(), m_simplestation);


			m_simplestation->m_cdrom->m_queued_responses = 1;
			
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;

			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			//printf("Created event CDROM_SETMODE_CMD...\n");

			break;

		case CDROM_SEEKL_CMD:
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00001000;
			printf("[CDROM] SeekL\n");
			m_cdrom_response_fifo_push(0x42, m_simplestation);
			m_simplestation->m_cdrom->m_queued_responses = 1;
			
			queued_fifo[0] = m_simplestation->m_cdrom->m_status_register.raw;
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;

			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);

			secondary_event.time = m_simplestation->time + 200000;
			secondary_event.func = &INT2;
			scheduler_push(secondary_event, m_simplestation);

			//printf("Created event CDROM_SEEKL_CMD...\n");
			break;

		case 0x13:
			printf("[CDROM] GetTN\n");
			m_cdrom_response_fifo_push(get_stat(), m_simplestation);
			m_cdrom_response_fifo_push(0x01, m_simplestation);
			m_cdrom_response_fifo_push(0x01, m_simplestation);
			m_simplestation->m_cdrom->m_queued_responses = 3;
			
			m_simplestation->m_cdrom->m_status_register.raw |= 0b00100000;
			

			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			
			break;

		case CDROM_TEST_CMD:
			m_cdrom_exec_test_subcmd(m_cdrom_parameter_fifo_pop(m_simplestation), m_simplestation);
			break;

		case CDROM_GETID_CMD:
			m_cdrom_response_fifo_push(get_stat(), m_simplestation);
			m_simplestation->m_cdrom->m_queued_responses = 1;

			
			queued_fifo[0] = 0x02;
			queued_fifo[1] = 0;
			queued_fifo[2] = 0;
			queued_fifo[3] = 0;
			queued_fifo[4] = 'S';
			queued_fifo[5] = 'C';
			queued_fifo[6] = 'E';
			queued_fifo[7] = 'A';
			
			primary_event.time = m_simplestation->time + 50401 + 18944;
			primary_event.func = &INT2;
			scheduler_push(primary_event, m_simplestation);
			//printf("Created event CDROM_GETID_CMD...\n");

			break;

		default:
			printf(RED "[CDROM] write: Unhandled CDROM Command: 0x%02X\n" NORMAL, m_cmd);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}


	for (int i = 0; i < 16; i++)
	{
		m_simplestation->m_cdrom->m_parameter_fifo[i] = 0;
	}

	m_simplestation->m_cdrom->m_parameter_fifo_index = 0;
}

void m_cdrom_exec_test_subcmd(uint8_t m_subcmd, m_simplestation_state *m_simplestation)
{
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

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
			
			primary_event.time = m_simplestation->time + 50401;
			primary_event.func = &INT3;
			
			scheduler_push(primary_event, m_simplestation);
			//printf("Created event INT3(yy,mm,dd,ver)...\n");
			break;

		default:
			printf(RED "[CDROM] write: Unhandled CDROM TEST Sub-command: 0x%02X\n" NORMAL, m_subcmd);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}
