#pragma once

#include <simplestation.h>
#include <ui/termcolour.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CPU_SPEED (44100 * 0x300)
#define READ_TIME_SINGLE (CPU_SPEED / 75)
#define READ_TIME_DOUBLE (CPU_SPEED / (2 * 75))

#define _1MS (CPU_SPEED / 1000)

#define INT3_TIME (_1MS)

#define SECTOR_SIZE 2352
#define READ_SIZE   0x818

typedef enum {
    GetStat = 0x01,
    SetLoc  = 0x02,
    Play    = 0x03,
    ReadN   = 0x06,
    Stop    = 0x08,
    Pause   = 0x09,
    Init    = 0x0A,
    Unmute  = 0x0C,
    SetFilter = 0x0D,
    SetMode = 0x0E,
    GetLocP = 0x11,
    GetTN   = 0x13,
    GetTD   = 0x14,
    SeekL   = 0x15,
    Test    = 0x19,
    GetID   = 0x1A,
    ReadS   = 0x1B,
    ReadTOC = 0x1E
} Command;

/* Seek parameters */
typedef struct {
    int mins, secs, sector;
} SeekParam;

/* --- CD-ROM registers --- */

typedef enum {
    CDDAOn     = 1 << 0,
    AutoPause  = 1 << 1,
    Report     = 1 << 2,
    XAFilter   = 1 << 3,
    Ignore     = 1 << 4,
    FullSector = 1 << 5,
    XAADPCMOn  = 1 << 6,
    Speed      = 1 << 7,
} Mode;

typedef enum {
    Error     = 1 << 0,
    MotorOn   = 1 << 1,
    SeekError = 1 << 2,
    IDError   = 1 << 3,
    ShellOpen = 1 << 4,
    Read      = 1 << 5,
    Seek      = 1 << 6,
    Play_     = 1 << 7,
} Status;


/* Function Definitions */
uint8_t m_cdrom_init(m_simplestation_state *m_simplestation);
void m_cdrom_exit(m_simplestation_state *m_simplestation);
void m_cdrom_setup(m_simplestation_state *m_simplestation);
