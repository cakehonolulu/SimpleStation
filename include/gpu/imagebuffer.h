#include <simplestation.h>
#include <stdint.h>
#include <stdlib.h>

uint16_t imageBuffer_Read(m_simplestation_state *m_simplestation, uint32_t x, uint32_t y);
void imageBuffer_Store(m_simplestation_state *m_simplestation, uint32_t word);
void imageBuffer_Reset(m_simplestation_state *m_simplestation, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void imageBuffer_Create(m_simplestation_state *m_simplestation);