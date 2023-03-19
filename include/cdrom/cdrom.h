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
#define CDROM_INIT_CMD          0x0A
#define CDROM_SETMODE_CMD       0x0E
#define CDROM_SEEKL_CMD         0x15
#define CDROM_TEST_CMD          0x19
#define CDROM_GETID_CMD         0x1A

/* CDROM Sub-Commands */
#define INT3_yy_mm_dd_ver       0x20

#define BCD_TO_DEC(bcd)         (((bcd >> 4) & 0xF) * 10 + (bcd & 0xF))

#define SPS                     75
#define SPM                     60


#define INT1                    1
#define INT2                    2
#define INT3                    3

typedef enum {
    UNKN = 0,
    READ = 5,
    SEEK = 6,
    PLAY = 7,
} cdrom_state;

typedef enum  {
    DataOnly800h = 0,
    WholeSector924h = 1,
} cdrom_sector_size ;

typedef enum  {
    Normal = 0,
    Double = 1,
} cdrom_speed;

/* Function Definitions */
uint8_t m_cdrom_init(m_simplestation_state *m_simplestation);
void m_cdrom_exit(m_simplestation_state *m_simplestation);
void m_cdrom_setup(m_simplestation_state *m_simplestation);
void m_cdrom_step(m_simplestation_state *m_simplestation);
void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation);
uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation);
void m_cdrom_exec_cmd(uint8_t m_cmd, m_simplestation_state *m_simplestation);
void m_cdrom_exec_test_subcmd(uint8_t m_subcmd, m_simplestation_state *m_simplestation);
