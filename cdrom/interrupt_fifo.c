#include <cdrom/interrupt_fifo.h>
#include <ui/termcolour.h>
#include <stdio.h>

void m_cdrom_interrupt_fifo_init(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->interrupt_fifo = malloc(sizeof(struct byte_queue));

    m_simplestation->m_cdrom->interrupt_fifo->items = malloc(INTERRUPT_FIFO_SIZE * sizeof(uint8_t));
    m_simplestation->m_cdrom->interrupt_fifo->maxsize = INTERRUPT_FIFO_SIZE;
    m_simplestation->m_cdrom->interrupt_fifo->front = 0;
    m_simplestation->m_cdrom->interrupt_fifo->rear = -1;
    m_simplestation->m_cdrom->interrupt_fifo->size = 0;
}

uint8_t m_cdrom_interrupt_fifo_size(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_cdrom->interrupt_fifo->size;
}

uint8_t m_cdrom_interrupt_fifo_is_empty(m_simplestation_state *m_simplestation)
{
    return !m_cdrom_interrupt_fifo_size(m_simplestation);
}

uint8_t m_cdrom_interrupt_fifo_front(m_simplestation_state *m_simplestation)
{
    if (m_cdrom_interrupt_fifo_is_empty(m_simplestation))
    {
        printf(BOLD RED "[CDROM] interrupt_front: Underflow detected, exiting..." NORMAL "\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    return m_simplestation->m_cdrom->interrupt_fifo->items[m_simplestation->m_cdrom->interrupt_fifo->front];
}

void m_cdrom_interrupt_fifo_push(uint8_t m_parameter, m_simplestation_state *m_simplestation)
{
    if (m_cdrom_interrupt_fifo_size(m_simplestation) == m_simplestation->m_cdrom->interrupt_fifo->maxsize)
    {
        printf(BOLD RED "[CDROM] interrupt_push: Overflow detected, exiting..." NORMAL "\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    m_simplestation->m_cdrom->interrupt_fifo->rear = (m_simplestation->m_cdrom->interrupt_fifo->rear + 1) % m_simplestation->m_cdrom->interrupt_fifo->maxsize;
    m_simplestation->m_cdrom->interrupt_fifo->items[m_simplestation->m_cdrom->interrupt_fifo->rear] = m_parameter;
    m_simplestation->m_cdrom->interrupt_fifo->size++;
}

uint8_t m_cdrom_interrupt_fifo_pop(m_simplestation_state *m_simplestation)
{
    uint8_t m_return = 0;

    if (m_cdrom_interrupt_fifo_is_empty(m_simplestation))
    {
        printf(BOLD RED "[CDROM] interrupt_pop: Underflow detected, exiting..." NORMAL "\n");
        m_simplestation_exit(m_simplestation, 1);
    }

    m_return = m_cdrom_interrupt_fifo_front(m_simplestation);
    
    m_simplestation->m_cdrom->interrupt_fifo->front = (m_simplestation->m_cdrom->interrupt_fifo->front + 1) % m_simplestation->m_cdrom->interrupt_fifo->maxsize;
    m_simplestation->m_cdrom->interrupt_fifo->size--;

    return m_return;
}

void m_interrupt_fifo_flush(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_cdrom->interrupt_fifo->front = 0;
    m_simplestation->m_cdrom->interrupt_fifo->rear = -1;
    m_simplestation->m_cdrom->interrupt_fifo->size = 0;
    memset(m_simplestation->m_cdrom->interrupt_fifo->items, 0, sizeof(m_simplestation->m_cdrom->interrupt_fifo->items));
}
