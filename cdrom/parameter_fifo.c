#include <cdrom/parameter_fifo.h>
#include <ui/termcolour.h>
#include <stdio.h>

void m_cdrom_parameter_fifo_init(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_cdrom->m_parameter_fifo, 0, sizeof(m_simplestation->m_cdrom->m_parameter_fifo));
    m_simplestation->m_cdrom->m_parameter_fifo_front = 0;
    m_simplestation->m_cdrom->m_parameter_fifo_rear = -1;
    m_simplestation->m_cdrom->m_parameter_fifo_count = 0;
}

uint8_t param_front(m_simplestation_state *m_simplestation)
{
   return m_simplestation->m_cdrom->m_parameter_fifo[m_simplestation->m_cdrom->m_parameter_fifo_front];
}

bool param_isempty(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->m_parameter_fifo_count == 0;
}

bool param_isfull(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->m_parameter_fifo_count == PARAM_FIFO_SZ;
}

int8_t param_size(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->m_parameter_fifo_count;
}

void param_push(uint8_t param, m_simplestation_state *m_simplestation)
{
    if (!param_isfull(m_simplestation))
    {
        if (m_simplestation->m_cdrom->m_parameter_fifo_rear == (PARAM_FIFO_SZ - 1))
        {
            m_simplestation->m_cdrom->m_parameter_fifo_rear = -1;
        }

        m_simplestation->m_cdrom->m_parameter_fifo[++m_simplestation->m_cdrom->m_parameter_fifo_rear] = param;
        
        m_simplestation->m_cdrom->m_parameter_fifo_count++;
    }
}

uint8_t param_pop(m_simplestation_state *m_simplestation)
{
    uint8_t param = m_simplestation->m_cdrom->m_parameter_fifo[m_simplestation->m_cdrom->m_parameter_fifo_front++];

    if (m_simplestation->m_cdrom->m_parameter_fifo_front == PARAM_FIFO_SZ)
    {
        m_simplestation->m_cdrom->m_parameter_fifo_front = 0;
    }

    m_simplestation->m_cdrom->m_parameter_fifo_count--;

    return param;
}

void clearParameters(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_cdrom->m_parameter_fifo, 0, sizeof(m_simplestation->m_cdrom->m_parameter_fifo));
    m_simplestation->m_cdrom->m_parameter_fifo_front = 0;
    m_simplestation->m_cdrom->m_parameter_fifo_rear = -1;
    m_simplestation->m_cdrom->m_parameter_fifo_count = 0;
}

void m_cdrom_parameter_fifo_push(uint8_t m_parameter, m_simplestation_state *m_simplestation)
{
    if (param_size(m_simplestation) >= PARAM_FIFO_SZ)
    {
        printf(BOLD RED "[CDROM] parameter_fifo_push: Parameter FIFO Overflow, exiting...!" NORMAL "\n");
    }

    param_push(m_parameter, m_simplestation);

    updateStatusRegister(m_simplestation);
}

uint8_t m_cdrom_parameter_fifo_pop(m_simplestation_state *m_simplestation)
{
    uint8_t m_parameter = 0;

    if (!param_isempty(m_simplestation))
    {
        m_parameter = param_front(m_simplestation);
        param_pop(m_simplestation);
        updateStatusRegister(m_simplestation);
    }

    return m_parameter;
}

/*
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

uint8_t m_cdrom_parameter_fifo_pop(m_simplestation_state *m_simplestation)
{
    uint8_t m_parameter = 0;

    // Only pop parameters if the current FIFO index's not 0
    if (m_simplestation->m_cdrom->m_parameter_fifo_index && m_simplestation->m_cdrom->m_parameter_fifo_index < 16)
    {
        m_parameter = m_simplestation->m_cdrom->m_parameter_fifo[--m_simplestation->m_cdrom->m_parameter_fifo_index];
    }
    else
    {
        printf(RED "[CDROM] parameter_fifo_push: FIFO Index Exceeded 16, aborting...!\n" NORMAL);
        m_simplestation_exit(m_simplestation, 1);
    }

    // (Un)Set the Parameter FIFO's PRMEMPT bit based on the current index's value
    m_simplestation->m_cdrom->m_status_register.prmempt = (m_simplestation->m_cdrom->m_parameter_fifo_index == 0);

    // Check if the FIFO is full (16 bytes have been written) and set PRMWRDY bit accordingly
    m_simplestation->m_cdrom->m_status_register.prmwrdy = (m_simplestation->m_cdrom->m_parameter_fifo_index < 16);    

    return m_parameter;
}
*/