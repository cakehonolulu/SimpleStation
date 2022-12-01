#include <memory/dma/dma.h>

uint8_t m_dma_init(m_simplestation_state *m_simplestation)
{
    uint8_t m_result = 0;

	m_simplestation->m_memory->m_dma = (m_psx_dma_t *) malloc(sizeof(m_psx_dma_t));
    
    if (m_simplestation->m_memory->m_dma)
    {
        m_simplestation->m_memory->m_dma->m_control_reg = 0x07654321;

        m_simplestation->m_memory->m_dma->m_irq_enable = false;

        m_simplestation->m_memory->m_dma->m_irq_channel_enable = 0;

        m_simplestation->m_memory->m_dma->m_irq_channel_flags = 0;

        m_simplestation->m_memory->m_dma->m_irq_force = false;
        
        m_simplestation->m_memory->m_dma->m_irq_dummy = 0;

        if (m_channels_init(m_simplestation) == 0)
        {
            m_simplestation->m_dma_state = ON;
        }
        else
        {
            printf("[DMA] channels_init: Failed to initialize PSX's DMA Channels Subsystem!\n");
            m_result = 1;
        }
    }
    else
    {
        m_result = 1;
    }

    return m_result;
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

            if (m_channel_get_active(m_simplestation, m_major))
            {
                m_dma_run(m_simplestation, m_major);
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

void m_dma_run(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    switch(m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_sync)
    {
        case linked_list:
#ifdef DEBUG_DMA
            printf(YELLOW "[DMA] run: Running DMA on channel %d (" MAGENTA "Linked List" YELLOW " approach)...\n" NORMAL, m_id);
#endif
            m_dma_run_linked_list(m_simplestation, m_id);
            break;

        default:
#ifdef DEBUG_DMA
            printf(YELLOW "[DMA] run: Running DMA on channel %d (" CYAN "Block" YELLOW " approach)...\n" NORMAL, m_id);
#endif
            m_dma_run_block(m_simplestation, m_id);
            break;
    }
}

void m_dma_run_block(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    int8_t m_increment = -4;
    uint32_t m_address;
    uint32_t m_current_address;
    uint32_t m_size;
    uint32_t m_source;

    if (m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_step == increment)
    {
        m_increment = 4;
    }

    m_address = m_channel_get_base(m_simplestation, m_id);

    m_size = m_channel_get_transfer_size(m_simplestation, m_id);

    if (m_size == 0)
    {
        printf(RED "[DMA] run_block: Failed to find transfer size!\n" NORMAL);
        m_simplestation_exit(m_simplestation, 1);
    }

    while (m_size > 0)
    {
        m_current_address = m_address & 0x1FFFFC;

        switch (m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_direction)
        {
            case to_ram:

                switch(m_id)
                {
                    case 6:

                        switch(m_size)
                        {
                            case 1:
                                m_source = 0xFFFFFF;
                                break;

                            default:
                                m_source = (m_address - 4) & 0x1FFFFF;
                                break;
                        }
                        break;

                    default:
                        printf(RED "[DMA] run_block: Unimplemented channel block copy to RAM\n" NORMAL);
                        m_simplestation_exit(m_simplestation, 1);
                        break;
                }

                m_memory_write(m_current_address, m_source, dword, m_simplestation);          
                break;

            case from_ram:
                m_source = m_memory_read(m_current_address, dword, m_simplestation);

                switch(m_id)
                {
                    case 2:
#ifdef DEBUG_DMA
                        printf(CYAN "[DMA] run_block: GPU Data Command 0x%08X\n" NORMAL, m_source);
#endif
                        break;

                    default:
                        printf(RED "[DMA] run_block: Unimplemented block copy from RAM\n" NORMAL);
                        m_simplestation_exit(m_simplestation, 1);
                        break;
                }
                
                break;

            default:
                break;
        }

        m_address += m_increment;
        m_size--;
    }

    m_channel_set_done(m_simplestation, m_id);
}

void m_dma_run_linked_list(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    uint32_t m_address = 0;
    uint32_t m_header = 0;
    uint32_t m_size = 0;
    uint32_t m_command = 0;

    if (m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_direction == to_ram)
    {
        printf(RED "[DMA] run_linked_list: Invalid direction!\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    if (m_id != 2)
    {
        printf(RED "[DMA] run_linked_list: Invalid channel!\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    m_address = m_channel_get_base(m_simplestation, m_id) & 0x1FFFFC;

    while (1)
    {
        m_header = m_memory_read(m_address, dword, m_simplestation);
        m_size = m_header >> 24;

        while (m_size > 0)
        {
            m_address = (m_address + 4) & 0x1FFFFC;
            m_command = m_memory_read(m_address, dword, m_simplestation);

            (void) m_command;

            m_size--;
        }

        if ((m_header & 0x800000) != 0)
        {
            break;
        }

        m_address = m_header & 0x1FFFFC;
    }

    m_channel_set_done(m_simplestation, m_id);
}

void m_dma_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_memory->m_dma)
	{
		// Free the DMA Buffer
		free(m_simplestation->m_memory->m_dma);
	}
}
