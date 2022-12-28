#include <simplestation.h>
#include <ui/termcolour.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t m_cdrom_init(m_simplestation_state *m_simplestation);
void m_cdrom_exit(m_simplestation_state *m_simplestation);

void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation);
uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation);
