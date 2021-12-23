#ifndef BIOS_H
#define BIOS_H

#include <simplestation.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Variables */
extern int8_t *m_bios_buffer;

/* Function definitions */
void m_bios_load(const char *m_bios_name);
void m_bios_close();

#endif /* BIOS_H */