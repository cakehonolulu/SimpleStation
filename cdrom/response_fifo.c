#include <cdrom/response_fifo.h>
#include <ui/termcolour.h>
#include <stdio.h>

void m_cdrom_response_fifo_init(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_cdrom->m_response_fifo, 0, sizeof(m_simplestation->m_cdrom->m_response_fifo));
    m_simplestation->m_cdrom->m_response_fifo_front = 0;
    m_simplestation->m_cdrom->m_response_fifo_rear = -1;
    m_simplestation->m_cdrom->m_response_fifo_count = 0;
}

uint8_t response_front(m_simplestation_state *m_simplestation)
{
   return m_simplestation->m_cdrom->m_response_fifo[m_simplestation->m_cdrom->m_response_fifo_front];
}

bool response_isempty(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->m_response_fifo_count == 0;
}

bool response_isfull(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->m_response_fifo_count == RESPONSE_FIFO_SZ;
}

int8_t response_size(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->m_response_fifo_count;
}

void response_push(uint8_t response, m_simplestation_state *m_simplestation)
{
    if (!response_isfull(m_simplestation))
    {
        if (m_simplestation->m_cdrom->m_response_fifo_rear == (RESPONSE_FIFO_SZ - 1))
        {
            m_simplestation->m_cdrom->m_response_fifo_rear = -1;
        }

        m_simplestation->m_cdrom->m_response_fifo[++m_simplestation->m_cdrom->m_response_fifo_rear] = response;
        
        m_simplestation->m_cdrom->m_response_fifo_count++;
    }
}

uint8_t response_pop(m_simplestation_state *m_simplestation)
{
    uint8_t response = m_simplestation->m_cdrom->m_response_fifo[m_simplestation->m_cdrom->m_response_fifo_front++];

    if (m_simplestation->m_cdrom->m_response_fifo_front == RESPONSE_FIFO_SZ)
    {
        m_simplestation->m_cdrom->m_response_fifo_front = 0;
    }

    m_simplestation->m_cdrom->m_response_fifo_count--;

    return response;
}

void clearResponses(m_simplestation_state *m_simplestation)
{
    memset(m_simplestation->m_cdrom->m_response_fifo, 0, sizeof(m_simplestation->m_cdrom->m_response_fifo));
    m_simplestation->m_cdrom->m_response_fifo_front = 0;
    m_simplestation->m_cdrom->m_response_fifo_rear = -1;
    m_simplestation->m_cdrom->m_response_fifo_count = 0;
}

void m_cdrom_response_fifo_push(uint8_t m_response, m_simplestation_state *m_simplestation)
{
    if (param_size(m_simplestation) >= RESPONSE_FIFO_SZ)
    {
        printf(BOLD RED "[CDROM] response_fifo_push: Response FIFO Overflow, exiting...!" NORMAL "\n");
    }

    response_push(m_response, m_simplestation);
}

uint8_t m_cdrom_response_fifo_pop(m_simplestation_state *m_simplestation)
{
    uint8_t m_response = 0;

    if (!response_isempty(m_simplestation))
    {
        m_response = response_front(m_simplestation);
        response_pop(m_simplestation);
    }

    return m_response;
}

/*
void m_cdrom_response_fifo_push(uint8_t m_response, m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_cdrom->m_response_fifo_index < 16)
    {
        // Push the response to the FIFO
        m_simplestation->m_cdrom->m_response_fifo[m_simplestation->m_cdrom->m_response_fifo_index++] = m_response;

        // We just buffered a response, FIFO isn't empty, therefore set RSLRRDY bit to 1 (!0 = !Empty, 0 = Empty)
        m_simplestation->m_cdrom->m_status_register.rslrrdy = 1;
    }
    else
    {
        printf(RED "[CDROM] response_fifo_push: FIFO Index Exceeded 16, aborting...!\n" NORMAL);
        m_simplestation_exit(m_simplestation, 1);
    }
}

uint8_t m_cdrom_response_fifo_pop(m_simplestation_state *m_simplestation)
{
    uint8_t m_response = 0;

    if (m_simplestation->m_cdrom->m_response_fifo_index < 16)
    {
        // Check if we're about to pop() the last response out of the FIFO
        if (m_simplestation->m_cdrom->m_response_fifo_index > 0)
        {
            if (m_simplestation->m_cdrom->m_response_fifo_index == 1)
            {
                m_simplestation->m_cdrom->m_status_register.rslrrdy = 0;
            }
            
            m_response = m_simplestation->m_cdrom->m_response_fifo[--m_simplestation->m_cdrom->m_response_fifo_index];
        }
        else
        {
            m_response = 0;
        }
    }
    else
    {
        printf(RED "[CDROM] response_fifo_pop: FIFO Index Exceeded 16, aborting...!\n" NORMAL);
        m_simplestation_exit(m_simplestation, 1);
    }

    return m_response;
}
*/