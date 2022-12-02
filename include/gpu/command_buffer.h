#include <ui/termcolour.h>
#include <simplestation.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Function Definitions */
uint8_t m_gpu_command_buffer_init(m_simplestation_state *m_simplestation);
void m_gpu_command_buffer_exit(m_simplestation_state *m_simplestation);
uint8_t m_gpu_command_buffer_get_current_length(m_simplestation_state *m_simplestation);
void m_gpu_command_buffer_clear(m_simplestation_state *m_simplestation);
void m_gpu_command_buffer_push_word(m_simplestation_state *m_simplestation, uint32_t m_word);
