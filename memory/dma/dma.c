#include <memory/dma/dma.h>

void m_dma_init(m_simplestation_state *m_simplestation)
{
	m_simplestation->m_memory->m_dma = (m_psx_dma_t *) malloc(sizeof(m_psx_dma_t));

	m_simplestation->m_memory->m_dma->m_control_reg = 0x07654321;

    m_simplestation->m_memory->m_dma->m_irq_enable = false;

    m_simplestation->m_memory->m_dma->m_irq_channel_enable = 0;

	m_simplestation->m_memory->m_dma->m_irq_channel_flags = 0;

	m_simplestation->m_memory->m_dma->m_irq_force = false;
	
	m_simplestation->m_memory->m_dma->m_irq_dummy = 0;

    m_channels_init(m_simplestation);

	m_simplestation->m_dma_state = ON;
}

uint32_t m_dma_read(uint32_t m_addr, m_simplestation_state *m_simplestation)
{
    uint8_t m_major = (m_addr & 0x70) >> 4;
    uint8_t m_minor = (m_addr & 0xF);

    uint32_t m_value = 0;

    switch (m_major)
    {
        case 0 ... 6:
            
            switch(m_minor)
            {
                case 0:
                    m_value = m_channel_get_base(m_simplestation, m_major);
                    break;

                case 4:
                    m_value = m_channel_get_block_control(m_simplestation, m_major);
                    break;

                case 8:
                    m_value = m_channel_get_control(m_simplestation, m_major);
                    break;

                default:
                    printf(RED "[MEM] dma_read: Unhandled read! (@ 0x%08X)\n" NORMAL, m_addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;

        case 7:
            switch (m_minor)
            {
                case 0:
                    m_value = m_simplestation->m_memory->m_dma->m_control_reg;
                    break;

                case 4:
                    m_value = m_get_interrupt(m_simplestation);
                    break;

                default:
                    printf(RED "[MEM] dma_read: Unhandled read! (@ 0x%08X)\n" NORMAL, m_addr);
                    m_value = m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;

        default:
            printf(RED "[MEM] dma_read: Unhandled read! (@ 0x%08X)\n" NORMAL, m_addr);
            m_value = m_simplestation_exit(m_simplestation, 1);
            break;
    }

    return m_value;
}

void m_dma_write(uint32_t m_addr, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    uint8_t m_major = (m_addr & 0x70) >> 4;
    uint8_t m_minor = (m_addr & 0xF);

    switch (m_major)
    {
        case 0 ... 6:

            switch(m_minor)
            {
                case 0:
                    m_channel_set_base(m_simplestation, m_value, m_major);
                    break;

                case 4:
                    m_channel_set_block_control(m_simplestation, m_value, m_major);
                    break;
                    
                case 8:
                    m_channel_set_control(m_simplestation, m_value, m_major);
                    break;

                default:
                    printf(RED "[MEM] dma_write: Unhandled write! (@ 0x%08X)\n" NORMAL, m_addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;
        
        case 7:
            switch (m_minor)
            {
                case 0:
                    m_simplestation->m_memory->m_dma->m_control_reg = m_value;
                    break;

                case 4:
                    m_set_interrupt(m_value, m_simplestation);
                    break;
                
                default:
                    printf(RED "[MEM] dma_write: Unhandled write! (@ 0x%08X)\n" NORMAL, m_addr);
                    m_simplestation_exit(m_simplestation, 1);
                    break;
            }
            break;

        default:
            printf(RED "[MEM] dma_write: Unhandled write! (@ 0x%08X)\n" NORMAL, m_addr);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

}

void m_set_interrupt(uint32_t m_val, m_simplestation_state *m_simplestation)
{
    m_simplestation->m_memory->m_dma->m_irq_dummy = (uint8_t) (m_val & 0x3F);
    m_simplestation->m_memory->m_dma->m_irq_force = ((m_val >> 15) & 1) != 0;
    m_simplestation->m_memory->m_dma->m_irq_channel_enable = (uint8_t) ((m_val >> 24) & 0x3F);
    m_simplestation->m_memory->m_dma->m_irq_enable = ((m_val >> 23) & 1) != 0;

    uint8_t m_ack = ((m_val >> 24) & 0x3F);

    m_simplestation->m_memory->m_dma->m_irq_channel_flags &= !m_ack;
}

bool m_irq(m_simplestation_state *m_simplestation)
{
    bool m_irq_channel = m_simplestation->m_memory->m_dma->m_irq_channel_flags & m_simplestation->m_memory->m_dma->m_irq_channel_enable;
    return ((m_simplestation->m_memory->m_dma->m_irq_force) || (m_simplestation->m_memory->m_dma->m_irq_enable && m_irq_channel != 0));
}

uint32_t m_get_interrupt(m_simplestation_state *m_simplestation)
{
    uint32_t m_interrupt = 0;

    m_interrupt |= ((uint32_t) m_simplestation->m_memory->m_dma->m_irq_dummy);
    m_interrupt |= ((uint32_t) m_simplestation->m_memory->m_dma->m_irq_force) << 15;
    m_interrupt |= ((uint32_t) m_simplestation->m_memory->m_dma->m_irq_channel_enable) << 16;
    m_interrupt |= ((uint32_t) m_simplestation->m_memory->m_dma->m_irq_enable) << 23;
    m_interrupt |= ((uint32_t) m_simplestation->m_memory->m_dma->m_irq_channel_flags) << 24;
    m_interrupt |= ((uint32_t) m_irq(m_simplestation)) << 24;
    
    return m_interrupt;
}

void m_dma_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_memory->m_dma)
	{
		// Free the DMA Buffer
		free(m_simplestation->m_memory->m_dma);
	}
}