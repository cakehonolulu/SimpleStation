#include <memory/dma/dma.h>

uint8_t m_channels_init(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_memory->m_dma->m_dma_channels, 0, (sizeof(m_psx_dma_channel_t) * 7));

    return 0;
}

uint32_t m_channel_get_control(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    uint32_t m_control = 0;

    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_direction) << 0;
    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_step) << 1;
    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_chop) << 8;
    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_sync) << 9;

    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_chop_dma_size) << 16;
    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_chop_cpu_size) << 20;
    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_enabled) << 24;

    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_trigger) << 28;

    m_control |= ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_dummy) << 29;

    return m_control;
}

void m_channel_set_control(m_simplestation_state *m_simplestation, uint32_t m_value, uint8_t m_id)
{
    if ((m_value & 1) != 0)
    {
        m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_direction = from_ram;
    }
    else
    {
        m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_direction = to_ram;
    }

    if (((m_value >> 1) & 1) != 0)
    {
        m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_step = decrement;
    }
    else
    {
        m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_step = increment;
    }

    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_chop = (((m_value >> 8) & 1) != 0);

    switch ((m_value >> 9) & 3)
    {
        case 0:
            m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_sync = manual;
            break;

        case 1:
            m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_sync = request;
            break;

        case 2:
            m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_sync = linked_list;
            break;

        default:
            printf(RED "[DMA] Unknown DMA Sync Mode!\n" NORMAL);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_chop_dma_size = (uint8_t) ((m_value >> 16) & 7);
    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_chop_cpu_size = (uint8_t) ((m_value >> 20) & 7);

    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_enabled = (((m_value >> 24) & 1) != 0);
    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_trigger = (((m_value >> 28) & 1) != 0);

    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_dummy = (uint8_t) ((m_value >> 29) & 3);
}

uint32_t m_channel_get_base(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    return m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_base;
}

void m_channel_set_base(m_simplestation_state *m_simplestation, uint32_t m_value, uint8_t m_id)
{
    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_base = (m_value & 0xFFFFFF);
}

uint32_t m_channel_get_block_control(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    return (((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_block_count) << 16)
            | ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_block_size);
}

void m_channel_set_block_control(m_simplestation_state *m_simplestation, uint32_t m_value, uint8_t m_id)
{
    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_block_size = (uint16_t) (m_value);
    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_block_count = (uint16_t) (m_value >> 16);
}

bool m_channel_get_active(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    bool m_trigger;

    if (m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_sync == manual)
    {
        m_trigger = m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_trigger;
    }
    else
    {
        m_trigger = true;
    }

    return m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_enabled && m_trigger;
}

void m_channel_set_done(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_enabled = false;
    m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_trigger = false;
}

uint32_t m_channel_get_transfer_size(m_simplestation_state *m_simplestation, uint8_t m_id)
{
    size_t m_size = 0;

    switch (m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_sync)
    {
        case manual:
            m_size = ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_block_size);
            break;
        
        case request:
            m_size = ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_block_size)
                     * ((uint32_t) m_simplestation->m_memory->m_dma->m_dma_channels[m_id].m_block_count);
            break;
        
        default:
            printf("[DMA] channel: Unknown transfer size\n");
            break;
    }

    return m_size;
}
