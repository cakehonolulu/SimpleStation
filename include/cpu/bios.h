#ifndef BIOS_H
#define BIOS_H

#include <simplestation.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Function definitions */
uint8_t m_bios_load(m_simplestation_state *m_simplestation, const char *m_bios_name);
void m_bios_exit(m_simplestation_state *m_simplestation);

#endif /* BIOS_H */