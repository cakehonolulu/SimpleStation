#include <gpu/gpu.h>

uint8_t m_gpu_init(m_simplestation_state *m_simplestation)
{
    uint8_t m_result = 0;

    m_simplestation->m_gpu = (m_psx_gpu_t *) malloc(sizeof(m_psx_gpu_t));

    if (m_simplestation->m_gpu)
    {
        m_result = 0;
    }
    else
    {
        m_result = 1;
    }

    memset(m_simplestation->m_gpu, 0, sizeof(m_psx_gpu_t));

    m_simplestation->m_gpu->m_field = top;

    return m_result;
}

void m_gpu_exit(m_simplestation_state *m_simplestation)
{
    if (m_simplestation->m_gpu_state)
    {
        free(m_simplestation->m_gpu);
    }

    m_simplestation->m_gpu_state = OFF;
}
