#include <cdrom/cdrom.h>

uint8_t m_cdrom_init(m_simplestation_state *m_simplestation)
{
    uint8_t m_result = 0;

    m_simplestation->m_cdrom = (m_psx_cdrom_t *) malloc(sizeof(m_psx_cdrom_t));

    if (m_simplestation->m_cdrom)
    {
        m_simplestation->m_cdrom_state = ON;
        memset(m_simplestation->m_cdrom, 0, sizeof(m_psx_cdrom_t));
    }
    else
    {
        m_result = 1;
    }

    return m_result;
}

void m_cdrom_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom)
    {
        free(m_simplestation->m_cdrom);
    }
}

void m_cdrom_write(uint8_t m_offset, uint32_t m_value, m_simplestation_state *m_simplestation)
{
    switch (m_offset)
    {
        case CDROM_STATUS_REG:
            printf(YELLOW "[CDROM] write: Status Register Write (Value: 0x%08X)\n", m_value);
            m_simplestation->m_cdrom->m_status = m_value;
            break;

        case CDROM_PARAM_FIFO_REG:
            printf(YELLOW "[CDROM] write: Parameter Fifo Register Write (Value: 0x%08X)\n", m_value);
            m_simplestation->m_cdrom->m_fifo = m_value;
            break;

        case CDROM_REQUEST_REG:
            printf(YELLOW "[CDROM] write: Request Register Write (Value: 0x%08X)\n", m_value);
            m_simplestation->m_cdrom->m_req = m_value;
            break;

        default:
            printf(RED "[CDROM] write: Unhandled CDROM Write (Offset: %d, Value: 0x%08X)\n" NORMAL, m_offset, m_value);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }
}

uint32_t m_cdrom_read(uint8_t m_offset, m_simplestation_state *m_simplestation)
{
    uint32_t m_value = 0;

    switch (m_offset)
    {
        default:
            printf(RED "[CDROM] read: Unhandled CDROM Read (Offset: %d)\n" NORMAL, m_offset);
            m_simplestation_exit(m_simplestation, 1);
            break;
    }

    return m_value;
}
