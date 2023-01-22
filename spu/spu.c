#include <spu/spu.h>
#include <ui/termcolour.h>

uint32_t m_spu_read(uint32_t m_address, m_simplestation_state *m_simplestation)
{
    uint32_t m_value = 0;

    switch (m_address)
    {
        default:
            printf(RED "[SPU] read: Unhandled SPU read (Address: 0x%08X)\n" NORMAL, m_address);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

    return m_value;
}

void m_spu_write(uint32_t m_address, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    switch (m_address)
    {
        default:
            printf(RED "[SPU] write: Unhandled SPU write (Address: 0x%08X, Value: 0x%08X)\n" NORMAL, m_address, m_value);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }
}
