#include <cdrom/parameter_fifo.h>
#include <ui/termcolour.h>
#include <stdio.h>

void m_cdrom_parameter_fifo_init(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->parameter_fifo = malloc(sizeof(struct byte_queue));

    m_simplestation->m_cdrom->parameter_fifo->items = malloc(PARAMETER_FIFO_SIZE * sizeof(uint8_t));
    m_simplestation->m_cdrom->parameter_fifo->maxsize = PARAMETER_FIFO_SIZE;
    m_simplestation->m_cdrom->parameter_fifo->front = 0;
    m_simplestation->m_cdrom->parameter_fifo->rear = -1;
    m_simplestation->m_cdrom->parameter_fifo->size = 0;
}

uint8_t m_cdrom_parameter_fifo_size(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->parameter_fifo->size;
}

uint8_t m_cdrom_parameter_fifo_is_empty(m_simplestation_state *m_simplestation)
{
    return !m_cdrom_parameter_fifo_size(m_simplestation);
}

uint8_t m_cdrom_parameter_fifo_front(m_simplestation_state *m_simplestation)
{
    if (m_cdrom_parameter_fifo_is_empty(m_simplestation))
    {
        printf(BOLD RED "[CDROM] parameter_front: Underflow detected, exiting..." NORMAL "\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    return m_simplestation->m_cdrom->parameter_fifo->items[m_simplestation->m_cdrom->parameter_fifo->front];
}

void m_cdrom_parameter_fifo_push(uint8_t m_parameter, m_simplestation_state *m_simplestation)
{
    if (m_cdrom_parameter_fifo_size(m_simplestation) == m_simplestation->m_cdrom->parameter_fifo->maxsize)
    {
        printf(BOLD RED "[CDROM] parameter_push: Overflow detected, exiting..." NORMAL "\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    m_simplestation->m_cdrom->parameter_fifo->rear = (m_simplestation->m_cdrom->parameter_fifo->rear + 1) % m_simplestation->m_cdrom->parameter_fifo->maxsize;
    m_simplestation->m_cdrom->parameter_fifo->items[m_simplestation->m_cdrom->parameter_fifo->rear] = m_parameter;
    m_simplestation->m_cdrom->parameter_fifo->size++;

    m_cdrom_update_status_register(m_simplestation);
}

uint8_t m_cdrom_parameter_fifo_pop(m_simplestation_state *m_simplestation)
{
    if (m_cdrom_parameter_fifo_is_empty(m_simplestation))
    {
        printf(BOLD RED "[CDROM] parameter_pop: Underflow detected, exiting..." NORMAL "\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    m_simplestation->m_cdrom->parameter_fifo->front = (m_simplestation->m_cdrom->parameter_fifo->front + 1) % m_simplestation->m_cdrom->parameter_fifo->maxsize;
    m_simplestation->m_cdrom->parameter_fifo->size--;

    m_cdrom_update_status_register(m_simplestation);
}

void m_parameter_fifo_flush(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->parameter_fifo->front = 0;
    m_simplestation->m_cdrom->parameter_fifo->rear = -1;
    m_simplestation->m_cdrom->parameter_fifo->size = 0;
}
