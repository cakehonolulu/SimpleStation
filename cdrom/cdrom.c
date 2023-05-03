#include <cdrom/cdrom.h>
#include <cpu/cpu.h>
#include <cpu/instructions.h>

FILE *cd = NULL;

size_t file_size = 0;

SeekParam seekParam = {0};

uint8_t readBuf[SECTOR_SIZE];
int readIdx = 0;

int queuedIRQ = 0;
bool oldCmdWasSeekL = true;

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


	m_simplestation->m_cdrom->queuedResp = cqueue_i_init();
	
	m_simplestation->m_cdrom->lateResp = cqueue_i_init();


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

    uint32_t seekTarget = mm + ss + sect; // Starts at 2s, subtract 150 sectors to get start

	seekTarget -= (2 * 75);

    //printf("[CDROM] Seeking to [%d:%d:%d] (LBA: %d)\n", seekParam.mins, seekParam.secs, seekParam.sector, seekTarget);

	printf("[CDROM] Reading from LBA %d\n", seekTarget);

	fseek(cd, seekTarget * SECTOR_SIZE, SEEK_SET);

	fread(&readBuf, SECTOR_SIZE, sizeof(uint8_t), cd);

    readIdx = (m_simplestation->m_cdrom->mode & (uint8_t)(FullSector)) ? 0x0C : 0x18;

    seekParam.sector++;

    /* Increment BCD values */
    if ((seekParam.sector & 0xF) == 10) { seekParam.sector += 0x10; seekParam.sector &= 0xF0; }

    if (seekParam.sector == 0x75) { seekParam.secs++; seekParam.sector = 0; }

    if ((seekParam.secs & 0xF) == 10) { seekParam.secs += 0x10; seekParam.secs &= 0xF0; }

    if (seekParam.secs == 0x60) { seekParam.mins++; seekParam.secs = 0; }

    if ((seekParam.mins & 0xF) == 10) { seekParam.mins += 0x10; seekParam.mins &= 0xF0; }

    //printf("[CDROM] Next seek to [%d:%d:%d]\n", seekParam.mins, seekParam.secs, seekParam.sector);
}

uint8_t readResponse(m_simplestation_state *m_simplestation) {
    //assert(!responseFIFO.empty());

	if (cqueue_i_empty(&m_simplestation->m_cdrom->responseFIFO))
	{
		return 0;
	}

    uint8_t data = (uint8_t) *cqueue_i_front(&m_simplestation->m_cdrom->responseFIFO);
	
	cqueue_i_pop(&m_simplestation->m_cdrom->responseFIFO);

    return data;
}

uint8_t getData8(m_simplestation_state *m_simplestation) {
    assert(readIdx && (readIdx < READ_SIZE));

    uint8_t data = readBuf[readIdx++];

    if (readIdx == READ_SIZE) readIdx = 0;

    return data;
}

uint32_t getData32(m_simplestation_state *m_simplestation) {
    assert(readIdx && (readIdx < READ_SIZE));

    uint32_t data;

    memcpy(&data, &readBuf[readIdx], 4);

    readIdx += 4;

	if (readIdx == READ_SIZE) readIdx = 0;

    return data;
}

void pushResponse(uint8_t data, m_simplestation_state *m_simplestation) {
	cqueue_i_push(&m_simplestation->m_cdrom->queuedResp, data);
}

void pushLateResponse(uint8_t data, m_simplestation_state *m_simplestation) {
	cqueue_i_push(&m_simplestation->m_cdrom->lateResp, data);
}

void clearParameters(m_simplestation_state *m_simplestation) {
    while (!cqueue_i_empty(&m_simplestation->m_cdrom->paramFIFO)) cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);
}

void clearResponse(m_simplestation_state *m_simplestation) {
    while (!cqueue_i_empty(&m_simplestation->m_cdrom->responseFIFO)) cqueue_i_pop(&m_simplestation->m_cdrom->responseFIFO);

    while (!cqueue_i_empty(&m_simplestation->m_cdrom->queuedResp)) cqueue_i_pop(&m_simplestation->m_cdrom->queuedResp);

    while (!cqueue_i_empty(&m_simplestation->m_cdrom->lateResp)) cqueue_i_pop(&m_simplestation->m_cdrom->lateResp);
}

void loadResponse(m_simplestation_state *m_simplestation) {
    while (!cqueue_i_empty(&m_simplestation->m_cdrom->queuedResp)) {
        cqueue_i_push(&m_simplestation->m_cdrom->responseFIFO, (uint8_t) *cqueue_i_front(&m_simplestation->m_cdrom->queuedResp));

        cqueue_i_pop(&m_simplestation->m_cdrom->queuedResp);
    }

    while (!cqueue_i_empty(&m_simplestation->m_cdrom->lateResp)) {
        cqueue_i_push(&m_simplestation->m_cdrom->queuedResp, (uint8_t) *cqueue_i_front(&m_simplestation->m_cdrom->lateResp));

        cqueue_i_pop(&m_simplestation->m_cdrom->lateResp);
    }
}

void INT1(m_simplestation_state *m_simplestation)
{
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

    //printf("[CDROM] INT2\n");

	if (m_simplestation->m_cdrom->iFlags)
	{
		//assert(!queuedIRQ);

		queuedIRQ = 1;

		return;
	}

    m_simplestation->m_cdrom->iFlags = (uint8_t) 1;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);

	loadResponse(m_simplestation);

	pushResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Read), m_simplestation);

	readSector(m_simplestation);

    if (m_simplestation->m_cdrom->mode & (uint8_t)(Speed)) {
		primary_event.time = m_simplestation->time + READ_TIME_DOUBLE;
    } else {
		primary_event.time = m_simplestation->time + READ_TIME_SINGLE;
    }

	primary_event.func = &INT1;
	scheduler_push(primary_event, m_simplestation);
}

void INT2(m_simplestation_state *m_simplestation)
{
    //printf("[CDROM] INT2\n");

	if (m_simplestation->m_cdrom->iFlags)
	{
		//assert(!queuedIRQ);

		queuedIRQ = 2;

		return;
	}

    m_simplestation->m_cdrom->iFlags = (uint8_t) 2;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);

	loadResponse(m_simplestation);
}

void INT3(m_simplestation_state *m_simplestation)
{
    //printf("[CDROM] INT3\n");

	if (m_simplestation->m_cdrom->iFlags)
	{
		//assert(!queuedIRQ);

		queuedIRQ = 3;

		return;
	}

    m_simplestation->m_cdrom->iFlags = (uint8_t) 3;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);

	loadResponse(m_simplestation);
}

void INT5(m_simplestation_state *m_simplestation)
{
    //printf("[CDROM] INT5\n");

	if (m_simplestation->m_cdrom->iFlags)
	{
		//assert(!queuedIRQ);

		queuedIRQ = 5;

		return;
	}

    m_simplestation->m_cdrom->iFlags = (uint8_t) 5;

    if (m_simplestation->m_cdrom->iEnable & m_simplestation->m_cdrom->iFlags) m_interrupts_request(CDROM, m_simplestation);

	loadResponse(m_simplestation);
}

/* 0x01 */
void cmdGetStat(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

    printf(BOLD MAGENTA "[CDROM] GetStat" NORMAL "\n");

   if (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO)) {
        /* Too few/many parameters, send error */
        clearParameters(m_simplestation);

        pushResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Error), m_simplestation);
        pushResponse(0x20, m_simplestation);

		primary_event.time = m_simplestation->time + INT3_TIME;
		primary_event.func = &INT5;
		scheduler_push(primary_event, m_simplestation);

        return;
    }

    // Send status
	if (m_simplestation->m_cdrom_in)
	{
		pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    	// Clear shell open flag
    	m_simplestation->m_cdrom->stat &= ~(uint8_t)(ShellOpen);
	}
	else
	{
		pushResponse(0b00010000, m_simplestation);
	}

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x02 */
void cmdSetLoc(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] SetLoc " NORMAL "\n");

    // Send status
    pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    /* Set minutes, seconds, sector */
    seekParam.mins   = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

    seekParam.secs   = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

    seekParam.sector = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);
	
    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x06 */
void cmdReadN(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] ReadN" NORMAL "\n");

	// Send status
	pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

    uint32_t int1Time = INT3_TIME + (m_simplestation->m_cdrom->mode & (uint8_t)(Speed)) ? READ_TIME_DOUBLE : READ_TIME_SINGLE;

	m_simplestation->m_cdrom->stat |= (uint8_t)(Read);
	primary_event.func = &INT1;
	primary_event.time = m_simplestation->time + int1Time;
	scheduler_push(primary_event, m_simplestation);

	pushLateResponse(m_simplestation->m_cdrom->stat, m_simplestation);
}

/* 0x08 */
void cmdStop(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] Stop" NORMAL "\n");

	scheduler_clean(m_simplestation);

	clearResponse(m_simplestation);

	pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

	primary_event.time = m_simplestation->time + CPU_SPEED;
	primary_event.func = &INT2;
	scheduler_push(primary_event, m_simplestation);

    m_simplestation->m_cdrom->stat &= ~(uint8_t)(MotorOn);
    m_simplestation->m_cdrom->stat &= ~(uint8_t)(Read);

    pushLateResponse(m_simplestation->m_cdrom->stat, m_simplestation);
}

/* 0x09 */
void cmdPause(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] Pause" NORMAL "\n");

	scheduler_clean(m_simplestation);

	clearResponse(m_simplestation);

	pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

	primary_event.time = m_simplestation->time + INT3_TIME + 70 * _1MS - 35 * _1MS * !!(m_simplestation->m_cdrom->mode & (uint8_t)(Speed));
	primary_event.func = &INT2;
	scheduler_push(primary_event, m_simplestation);

    m_simplestation->m_cdrom->stat &= ~(uint8_t)(Read);

    // Send status
    pushLateResponse(m_simplestation->m_cdrom->stat, m_simplestation);
}

/* 0x0A */
void cmdInit(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] Init" NORMAL "\n");

   if (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO)) {
        /* Too few/many parameters, send error */
        clearParameters(m_simplestation);

        pushResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Error), m_simplestation);
        pushResponse(0x20, m_simplestation);

		primary_event.time = m_simplestation->time + INT3_TIME;
		primary_event.func = &INT5;
		scheduler_push(primary_event, m_simplestation);

        return;
    }

    m_simplestation->m_cdrom->stat = (uint8_t)(MotorOn);

	// Send mode
	pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

    m_simplestation->m_cdrom->mode = (uint8_t)(FullSector);

    // Send mode
    pushLateResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT2
	primary_event.time = m_simplestation->time + INT3_TIME + 120 * _1MS;
	primary_event.func = &INT2;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x0C */
void cmdUnmute(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] Unmute" NORMAL "\n");

	pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x0D */
void cmdSetFilter(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] SetFilter" NORMAL "\n");

	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

    // Send status
	pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

}

/* 0x0E */
void cmdSetMode(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	// Send status
    pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    m_simplestation->m_cdrom->mode = *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);
	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

	printf(BOLD MAGENTA "[CDROM] SetMode (0x%X) " NORMAL "\n", m_simplestation->m_cdrom->mode);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x11 */
void cmdGetLocP(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] GetLocP" NORMAL "\n");

    pushResponse(0x01, m_simplestation);
    pushResponse(0x01, m_simplestation);
    pushResponse(seekParam.mins, m_simplestation);
    pushResponse(seekParam.secs, m_simplestation);
    pushResponse(seekParam.sector, m_simplestation);
    pushResponse(seekParam.mins, m_simplestation);
    pushResponse(seekParam.secs, m_simplestation);
    pushResponse(seekParam.sector, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x13 */
void cmdGetTN(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] GetTN" NORMAL "\n");

   if (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO)) {
        /* Too few/many parameters, send error */
        clearParameters(m_simplestation);

        pushResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Error), m_simplestation);
        pushResponse(0x20, m_simplestation);

		primary_event.time = m_simplestation->time + INT3_TIME;
		primary_event.func = &INT5;
		scheduler_push(primary_event, m_simplestation);

        return;
    }
    pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);
    pushResponse(0x01, m_simplestation);
    pushResponse(0x01, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x14 */
void cmdGetTD(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] GetTD" NORMAL "\n");

   if (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO) != 1) {
        /* Too few/many parameters, send error */
        clearParameters(m_simplestation);

        pushResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Error), m_simplestation);
        pushResponse(0x20, m_simplestation);

		primary_event.time = m_simplestation->time + INT3_TIME;
		primary_event.func = &INT5;
		scheduler_push(primary_event, m_simplestation);

        return;
    }

	uint8_t track = (uint8_t) *cqueue_i_front(&m_simplestation->m_cdrom->paramFIFO);

	cqueue_i_pop(&m_simplestation->m_cdrom->paramFIFO);

	if (track > 0x26) {
        /* Too few/many parameters, send error */
		clearParameters(m_simplestation);

        pushResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Error), m_simplestation);
        pushResponse(0x20, m_simplestation);

		primary_event.time = m_simplestation->time + INT3_TIME;
		primary_event.func = &INT5;
		scheduler_push(primary_event, m_simplestation);

        return;
    }

    pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);
    pushResponse(0, m_simplestation);
    pushResponse(0, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);
}


/* 0x15 */
void cmdSeekL(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");

	printf(BOLD MAGENTA "[CDROM] SeekL" NORMAL "\n");

    // Send status
    pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

    // Send status
    pushLateResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Seek), m_simplestation);

    // Send INT2
	primary_event.time = m_simplestation->time + INT3_TIME + 2 * _1MS;
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
			printf(BOLD MAGENTA "[CDROM] INT3(yy,mm,dd,ver)" NORMAL "\n");
			pushResponse(0x94, m_simplestation);
			pushResponse(0x09, m_simplestation);
			pushResponse(0x19, m_simplestation);
			pushResponse(0xC0, m_simplestation);

			primary_event.time = m_simplestation->time + INT3_TIME;
			primary_event.func = &INT3;
			scheduler_push(primary_event, m_simplestation);
			break;

		default:
			printf(BOLD RED "[CDROM] Unhandled TEST Sub-command: 0x%x" NORMAL "\n", subfunction);
			m_simplestation_exit(m_simplestation, 1);
			break;
	}
}

/* 0x1A */
void cmdGetID(m_simplestation_state *m_simplestation) {
	event_t primary_event;
	strcpy(primary_event.subsystem, "CDROM");
    printf(BOLD MAGENTA "[CDROM] GetID" NORMAL "\n");

   if (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO)) {
        /* Too few/many parameters, send error */
        clearParameters(m_simplestation);

        pushResponse(m_simplestation->m_cdrom->stat | (uint8_t)(Error), m_simplestation);
        pushResponse(0x20, m_simplestation);

		primary_event.time = m_simplestation->time + INT3_TIME;
		primary_event.func = &INT5;
		scheduler_push(primary_event, m_simplestation);

        return;
    }

    // Send status
	pushResponse(m_simplestation->m_cdrom->stat, m_simplestation);

    // Send INT3
	primary_event.time = m_simplestation->time + INT3_TIME;
	primary_event.func = &INT3;
	scheduler_push(primary_event, m_simplestation);

    /* Licensed, Mode2 */
    pushResponse(0x02, m_simplestation);
    pushResponse(0x00, m_simplestation);
    pushResponse(0x20, m_simplestation);
    pushResponse(0x00, m_simplestation);

    pushResponse('S', m_simplestation);
    pushResponse('C', m_simplestation);
    pushResponse('E', m_simplestation);
    pushResponse('A', m_simplestation);

    // Send INT2
	primary_event.time = m_simplestation->time + INT3_TIME + 30000;
	primary_event.func = &INT2;
	scheduler_push(primary_event, m_simplestation);
}

/* 0x1B */
void cmdReadS(m_simplestation_state *m_simplestation) {

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

		case Stop:
			cmdStop(m_simplestation);
			break;

		case Pause:
			cmdPause(m_simplestation);
			break;

		case Init:
			cmdInit(m_simplestation);
			break;
		
		case Unmute:
			cmdUnmute(m_simplestation);
			break;

		case SetFilter:
			cmdSetFilter(m_simplestation);
			break;

		case SetMode:
			cmdSetMode(m_simplestation);
			break;

		case GetLocP:
			cmdGetLocP(m_simplestation);
			break;

		case GetTN:
			cmdGetTN(m_simplestation);
			break;
		
		case GetTD:
			cmdGetTD(m_simplestation);
			break;

		case SeekL:
			oldCmdWasSeekL = true;
			cmdSeekL(m_simplestation);
			break;

		case Test:
			cmdTest(m_simplestation);
			break;

		case GetID:
			cmdGetID(m_simplestation);
			break;

		case ReadS:
			cmdReadN(m_simplestation);
			break;

        default:
            printf(BOLD RED "[CDROM] Unhandled command 0x%02X" NORMAL "\n", m_simplestation->m_cdrom->cmd);

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
			//printf("[CDROM] 8-bit read @ STATUS\n");

			m_value = m_simplestation->m_cdrom->index_;
            m_value |= cqueue_i_empty(&m_simplestation->m_cdrom->paramFIFO) << 3;        // Parameter FIFO empty
            m_value |= (cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO) != 16) << 4; // Parameter FIFO not full
            m_value |= !cqueue_i_empty(&m_simplestation->m_cdrom->responseFIFO) << 5;    // Response FIFO not empty
			m_value |= (readIdx && (readIdx < READ_SIZE)) << 6;  // Data FIFO not empty

			break;

		case 1:
			//printf("[CDROM] 8-bit read @ RESPONSE\n");

            m_value = readResponse(m_simplestation);
			break;

		case 2:
			m_value = getData8(m_simplestation);
			break;

		case 3:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
                    //printf("[CDROM] 8-bit read @ IE\n");
                    m_value = m_simplestation->m_cdrom->iEnable;
					break;

				case 1:
					//printf("[CDROM] 8-bit read @ IF\n");
                    m_value = m_simplestation->m_cdrom->iFlags | 0xE0;
					break;

				default:
					printf(BOLD RED "[CDROM] Unhandled 8-bit read at Offset %d (Index: %d)" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_);
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
			//printf("[CDROM] 8-bit write @ INDEX = 0x%02X\n", m_value);

            m_simplestation->m_cdrom->index_ = m_value & 3;
			break;

		case 1:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
					//printf("[CDROM] 8-bit write @ CMD = 0x%02X\n", m_value);

                    doCmd(m_value, m_simplestation);
					break;

				case 3:
					break;

				default:
					printf(BOLD RED "[CDROM] Unhandled 8-bit write at Offset: %u (Index: %d) = 0x%02X" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_, m_value);
					break;
			}
			break;
		
		case 2:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
					//printf("[CDROM] 8-bit write @ PARAM = 0x%02X\n", m_value);

                    assert(cqueue_i_size(&m_simplestation->m_cdrom->paramFIFO) < 16);

					cqueue_i_push(&m_simplestation->m_cdrom->paramFIFO, m_value);
					break;

				case 1:
					//printf("[CDROM] 8-bit write @ IE = 0x%02X\n", m_value);

                    m_simplestation->m_cdrom->iEnable = m_value & 0x1F;
					break;

				case 2:
					break;

				case 3:
					break;

				default:
					printf(BOLD RED "[CDROM] Unhandled 8-bit write at Offset: %u (Index: %d) = 0x%02X" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_, m_value);

					m_simplestation_exit(m_simplestation, 1);
					break;
			}
			break;

		case 3:
			switch (m_simplestation->m_cdrom->index_)
			{
				case 0:
                    // printf("[CDROM] 8-bit write @ REQUEST = 0x%02X\n", m_value);
                    break;

				case 1:
                    //printf("[CDROM] 8-bit write @ IF = 0x%02X\n", m_value);

                    m_simplestation->m_cdrom->iFlags &= (~m_value & 0x1F);

					if (!m_simplestation->m_cdrom->iFlags && queuedIRQ) {
						event_t primary_event;
						strcpy(primary_event.subsystem, "CDROM");

						switch (queuedIRQ) {
							case 1:
								primary_event.func = &INT1;
								break;

							case 2:
								primary_event.func = &INT2;
								break;

							case 3:
								primary_event.func = &INT3;
								break;
							
							case 5:
								primary_event.func = &INT5;
								break;
							
							default:
								printf("This should be unreachable\n");
								m_simplestation_exit(m_simplestation, 1);
								break;
						}
						
                        // Send queued INT
						primary_event.time = m_simplestation->time + _1MS;
						scheduler_push(primary_event, m_simplestation);

                        queuedIRQ = 0;
                    }
                    break;

				case 2:
					break;

				case 3:
					break;

				default:
					printf(BOLD RED "[CDROM] Unhandled 8-bit write at Offset: %u (Index: %d) = 0x%02X" NORMAL "\n", m_offset, m_simplestation->m_cdrom->index_, m_value);
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
