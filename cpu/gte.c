#include <cpu/gte.h>

uint8_t gte_init(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gte = malloc(sizeof(m_psx_gte_t));
}

uint8_t gte_exit(m_simplestation_state *m_simplestation)
{
    free(m_simplestation->m_gte);
}
