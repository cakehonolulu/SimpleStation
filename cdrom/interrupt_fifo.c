#include <cdrom/interrupt_fifo.h>
#include <ui/termcolour.h>
#include <stdio.h>

void m_cdrom_interrupt_fifo_init(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_cdrom->m_interrupt_fifo, 0, sizeof(m_simplestation->m_cdrom->m_interrupt_fifo));
    m_simplestation->m_cdrom->m_interrupt_fifo_index = 0;
}

void m_cdrom_interrupt_fifo_push(uint8_t m_interrupt, m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom->m_interrupt_fifo_index >= 16)
    {
        printf(RED "[CDROM] interrupt_fifo_push: FIFO Index Exceeded 16, aborting...!\n" NORMAL);
        m_simplestation_exit(m_simplestation, 1);
    }
    else
    {
        // Push the interrupt to the FIFO
        m_simplestation->m_cdrom->m_interrupt_fifo[m_simplestation->m_cdrom->m_interrupt_fifo_index] = m_interrupt;
        m_simplestation->m_cdrom->m_interrupt_fifo_index++;
    }

    m_cdrom_update_status_register(m_simplestation);
}

void m_interrupt_fifo_flush(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_cdrom->m_interrupt_fifo, 0, sizeof(m_simplestation->m_cdrom->m_interrupt_fifo));
    m_simplestation->m_cdrom->m_interrupt_fifo_index = 0;
}

uint8_t m_cdrom_interrupt_fifo_pop(m_simplestation_state *m_simplestation)
{
    uint8_t m_interrupt = 0;

    if (m_simplestation->m_cdrom->m_interrupt_fifo_index >= 15)
    {
        printf(RED "[CDROM] interrupt_fifo_pop: FIFO Index Exceeded 16, aborting...!\n" NORMAL);
        m_simplestation_exit(m_simplestation, 1);
    }
    else
    {
        // Only pop interrupts if the current FIFO index's not 0
        if (m_simplestation->m_cdrom->m_interrupt_fifo_index != 0)
        {
            m_interrupt = m_cdrom_interrupt_fifo_front;

            for (int i = 0; i < m_simplestation->m_cdrom->m_interrupt_fifo_index; i++)
            {
                m_simplestation->m_cdrom->m_interrupt_fifo[i] = m_simplestation->m_cdrom->m_interrupt_fifo[i + 1];
            }

            if (m_simplestation->m_cdrom->m_interrupt_fifo_index != 0) m_simplestation->m_cdrom->m_interrupt_fifo_index--;
        }
    }

    return m_interrupt;
}