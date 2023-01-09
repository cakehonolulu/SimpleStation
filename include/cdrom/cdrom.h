#include <simplestation.h>
#include <ui/termcolour.h>
#include <stdlib.h>
#include <stdio.h>

#define CDROM_STATUS_REG        0x00
#define CDROM_COMMAND_REG       0x01
#define CDROM_PARAM_FIFO_REG	0x02
#define CDROM_REQUEST_REG		0x03

uint8_t m_cdrom_init(m_simplestation_state *m_simplestation);
void m_cdrom_exit(m_simplestation_state *m_simplestation);

void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation);
uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation);