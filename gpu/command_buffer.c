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

void m_gpu_command_buffer_push_word(m_simplestation_state *m_simplestation, uint32_t m_word)
{
    m_simplestation->m_gpu->m_gpu_command_buffer->m_buffer[m_gpu_command_buffer_get_current_length(m_simplestation)] = m_word;
    m_simplestation->m_gpu->m_gpu_command_buffer->m_length++;
}

uint8_t m_gpu_command_buffer_get_current_length(m_simplestation_state *m_simplestation)
{
    return m_simplestation->m_gpu->m_gpu_command_buffer->m_length;
}

void m_gpu_command_buffer_set_current_length(m_simplestation_state *m_simplestation, uint8_t m_length)
{
    if (m_length > 12)
    {
        printf(RED "[GPU] command_buffer_set_current_length: Invalid size!\n");
        m_simplestation_exit(m_simplestation, 1);
    }
    
    m_simplestation->m_gpu->m_gpu_command_buffer->m_length = m_length;
}

void m_gpu_command_buffer_exit(m_simplestation_state *m_simplestation)
{
    free(m_simplestation->m_gpu->m_gpu_command_buffer);
}
