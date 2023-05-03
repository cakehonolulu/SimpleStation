#include <scheduler/scheduler.h>

// Thanks @liuk7071

#define MAX_ENTRIES 128

extern event_t *events;

uint8_t scheduler_init(m_simplestation_state *m_simplestation)
{
    uint8_t ret = 1;

    events = calloc(MAX_ENTRIES, sizeof(event_t));

    m_simplestation->scheduled = 0;
    m_simplestation->time = 0;

    return ret;
}

void scheduler_push(event_t event, m_simplestation_state *m_simplestation)
{
    if (m_simplestation->scheduled < MAX_ENTRIES)
    {
        if (event.func == NULL)
        {
            printf("Pushed event has NULL function, exiting...\n");
            m_simplestation_exit(m_simplestation, 1);
        }
        events[m_simplestation->scheduled] = event;
        m_simplestation->scheduled++;
    }
    else
    {
        printf("[SCHED] push: Queue full!\n");
    }
}

void scheduler_clean(m_simplestation_state *m_simplestation)
{
    memset(events, sizeof(event_t), MAX_ENTRIES);

    m_simplestation->scheduled = 0;
}

void scheduler_tick(int cycles, m_simplestation_state *m_simplestation)
{
    //printf("time: %ld, cycles to add: %d\n", m_simplestation->time, cycles);
    m_simplestation->time += cycles;

    int executed = 0;

    //printf("Scheduled events: %d\n", m_simplestation->scheduled);

    if (m_simplestation->scheduled)
    {
        for (int i = 0; i < m_simplestation->scheduled; i++)
        {
            if (m_simplestation->time >= events[i].time)
            {
                if (events[i].func != NULL) events[i].func(m_simplestation);
                memset(&events[i], 0, sizeof(event_t));
                executed++;
                m_simplestation->scheduled--;

                // This acts like a POP
                for (int j = 0; j < 63; j++)
                {
                    events[j] = events[j + 1];
                }
            }
            else
            {
                break;
            }
        }
    }
    
}

void scheduler_exit(m_simplestation_state *m_simplestation)
{
    free(events);
}
