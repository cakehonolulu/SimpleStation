#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <cpu/cpu.h>
#include <ui/termcolour.h>
#include <stdint.h>
#include <stdio.h>

/* Function definitions */
void m_printregs(m_simplestation_state *m_simplestation);

#endif /* DEBUGGER_H */