#include <gpu/command_buffer.h>

uint8_t m_gpu_command_buffer_init(m_simplestation_state *m_simplestation)
{
    uint8_t m_return = 0;

    m_simplestation->m_gpu->m_gpu_command_buffer = (m_psx_gpu_command_buffer_t *) malloc(sizeof(m_psx_gpu_command_buffer_t));

    if (!m_simplestation->m_gpu->m_gpu_command_buffer)
    {
        m_return = 1;
    }
    else
    {
        memset(m_simplestation->m_gpu->m_gpu_command_buffer, 0, sizeof(m_psx_gpu_command_buffer_t));
    }

    return m_return;
}

void m_gpu_command_buffer_exit(m_simplestation_state *m_simplestation)
{
    free(m_simplestation->m_gpu->m_gpu_command_buffer);
}
