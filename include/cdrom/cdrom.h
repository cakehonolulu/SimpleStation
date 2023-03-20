#pragma once

#include <simplestation.h>
#include <ui/termcolour.h>
#include <stdlib.h>
#include <stdio.h>

#define CDROM_STATUS_REG        0x00
#define CDROM_COMMAND_REG       0x01
#define CDROM_PARAM_FIFO_REG	0x02
#define CDROM_REQUEST_REG		0x03

/* CDROM Commands */
#define CDROM_GETSTAT_CMD       0x01
#define CDROM_SETLOC_CMD        0x02
#define CDROM_READN_CMD         0x06
#define CDROM_PAUSE_CMD         0x09
#define CDROM_SETMODE_CMD       0x0E
#define CDROM_SEEKL_CMD         0x15
#define CDROM_TEST_CMD          0x19
#define CDROM_GETID_CMD         0x1A

typedef enum {
    Unknown = 0,
    Reading = 5,
    Seeking = 6,
    Playing = 7,
} CDROMState;

typedef enum {
    DataOnly800h = 0,
    WholeSector924h = 1,
} CDROMModeSectorSize;

typedef enum {
    Normal = 0,
    Double = 1,
} CDROMModeSpeed;

/*
    Mode2/Form1 (CD-XA)
    000h 0Ch  Sync
    00Ch 4    Header (Minute,Second,Sector,Mode=02h)
    010h 4    Sub-Header (File, Channel, Submode AND DFh, Codinginfo)
    014h 4    Copy of Sub-Header
    018h 800h Data (2048 bytes)
    818h 4    EDC (checksum accross [010h..817h])
    81Ch 114h ECC (error correction codes)
*/
typedef struct {
    uint8_t sync[12];
    uint8_t header[4];
    uint8_t subheader[4];
    uint8_t subheaderCopy[4];
    uint8_t data[2048];
    uint8_t EDC[4];
    uint8_t ECC[276];
} CDSector;

const uint32_t SecondsPerMinute = 60;
const uint32_t SectorsPerSecond = 75;
const uint32_t SystemClocksPerCDROMInt1SingleSpeed=2352;
const uint32_t SystemClocksPerCDROMInt1DoubleSpeed=2352/2;

#define BCD_DECODE(value) (((value >> 4) & 0xF) * 10 + (value & 0xF))

/* Function Definitions */
uint8_t m_cdrom_init(m_simplestation_state *m_simplestation);
void m_cdrom_exit(m_simplestation_state *m_simplestation);
void m_cdrom_setup(m_simplestation_state *m_simplestation);
void m_cdrom_step(m_simplestation_state *m_simplestation);
void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation);
uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation);
void m_cdrom_exec_cmd(uint8_t m_cmd, m_simplestation_state *m_simplestation);
void m_cdrom_exec_test_subcmd(uint8_t m_subcmd, m_simplestation_state *m_simplestation);
