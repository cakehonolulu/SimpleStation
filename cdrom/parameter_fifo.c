#include <cdrom/parameter_fifo.h>
#include <ui/termcolour.h>

void m_cdrom_parameter_fifo_init(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_cdrom->m_parameter_fifo, 0, sizeof(m_simplestation->m_cdrom->m_parameter_fifo));
    m_simplestation->m_cdrom->m_parameter_fifo_index = 0;
}

void m_cdrom_parameter_fifo_push(uint8_t m_parameter, m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom->m_parameter_fifo_index < 16)
    {
        // Push the parameter to the FIFO
        m_simplestation->m_cdrom->m_parameter_fifo[m_simplestation->m_cdrom->m_parameter_fifo_index++] = m_parameter;

        // We just buffered a parameter, FIFO isn't empty, therefore set PRMEMPT bit to 0 (0 != Empty, 1 = Empty)
        m_simplestation->m_cdrom->m_status_register.prmempt = 0;

        // Check if the FIFO is full (16 bytes have been written) and set PRMWRDY bit accordingly
        m_simplestation->m_cdrom->m_status_register.prmwrdy = (m_simplestation->m_cdrom->m_parameter_fifo_index < 16);
    }
    else
    {
        printf(RED "[CDROM] parameter_fifo_push: FIFO Index Exceeded 16, aborting...!\n" NORMAL);
        m_simplestation_exit(m_simplestation, 1);
    }
}
