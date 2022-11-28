#include <memory/dma.h>

void m_dma_init(m_simplestation_state *m_simplestation)
{
	m_simplestation->m_memory->m_dma = (m_psx_dma_t *) malloc(sizeof(m_psx_dma_t));

	m_simplestation->m_memory->m_dma->m_control_reg = 0x07654321;

	m_simplestation->m_dma_state = ON;
}

void m_dma_write(uint32_t m_addr, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    switch (m_addr)
    {
        case 0x70:
            m_simplestation->m_memory->m_dma->m_control_reg = m_value;
            break;

        default:
            printf(RED "[MEM] dma_write: Unhandled write! (@ 0x%08X)\n" NORMAL, m_addr);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

}

uint32_t m_dma_read(uint32_t m_addr, m_simplestation_state *m_simplestation)
{
    uint32_t m_value;

    switch (m_addr)
    {
        case 0x70:
            m_value = m_simplestation->m_memory->m_dma->m_control_reg;
            break;

        default:
            printf(RED "[MEM] dma_read: Unhandled read! (@ 0x%08X)\n" NORMAL, m_addr);
            m_value = m_simplestation_exit(m_simplestation, 1);
            break;
    }

    return m_value;
}
