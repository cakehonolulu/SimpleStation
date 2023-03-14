#include <scheduler/scheduler.h>

// Thanks @liuk7071

#define MAX_ENTRIES 64

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
		printf("scheduler_push: event.cycle_target is %ld\n", events[m_simplestation->scheduled].time);
		printf("scheduler_push: event.subsystem is %s\n", events[m_simplestation->scheduled].subsystem);
        events[m_simplestation->scheduled] = event;
        m_simplestation->scheduled++;
    }
    else
    {
        printf("[SCHED] push: Queue full!\n");
    }
}

void scheduler_tick(int cycles, m_simplestation_state *m_simplestation)
{
    //printf("time: %ld, cycles to add: %d\n", m_simplestation->time, cycles);
    m_simplestation->time += cycles;

    int executed = 0;

    //printf("Scheduled events: %d\n", m_simplestation->scheduled);

    for (int i = 0; i < m_simplestation->scheduled; i++)
    {
        //printf("m_simplestation->scheduled: %d\n", m_simplestation->scheduled);
        //printf("time: %ld, scheduled time: %ld\n", m_simplestation->time, events[i].time);
		if (m_simplestation->time >= events[i].time)
        {
            printf("Executing from %s\n", events[i].subsystem);
            events[i].func(m_simplestation);
            events[i].func = NULL;
            executed++;
        }
        else
        {
            break;
        }
    }

    m_simplestation->scheduled -= executed;
}

void scheduler_exit(m_simplestation_state *m_simplestation)
{
    free(events);
}
