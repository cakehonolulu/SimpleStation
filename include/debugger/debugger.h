#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <cpu/cpu.h>
#include <memory/memory.h>
#include <cpu/instructions.h>
#include <ui/termcolour.h>
#include <stdint.h>
#include <stdio.h>

/* Function definitions */
void m_printregs(m_simplestation_state *m_simplestation);
void m_debugger(m_simplestation_state *m_simplestation);

#endif /* DEBUGGER_H */