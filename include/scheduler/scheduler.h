#pragma once

#include <simplestation.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Function Definitions */
uint8_t scheduler_init(m_simplestation_state *m_simplestation);
void scheduler_push(event_t event, m_simplestation_state *m_simplestation);
void scheduler_exit(m_simplestation_state *m_simplestation);
