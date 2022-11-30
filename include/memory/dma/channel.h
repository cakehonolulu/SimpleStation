#include <memory/dma/dma.h>

/* Function definitions */
void m_channels_init(m_simplestation_state *m_simplestation);
uint32_t m_channel_get_control(m_simplestation_state *m_simplestation, uint8_t m_id);
void m_channel_set_control(m_simplestation_state *m_simplestation, uint32_t m_value, uint8_t m_id);
uint32_t m_channel_get_base(m_simplestation_state *m_simplestation, uint8_t m_id);
void m_channel_set_base(m_simplestation_state *m_simplestation, uint32_t m_value, uint8_t m_id);
uint32_t m_channel_get_block_control(m_simplestation_state *m_simplestation, uint8_t m_id);
void m_channel_set_block_control(m_simplestation_state *m_simplestation, uint32_t m_value, uint8_t m_id);
bool m_channel_get_active(m_simplestation_state *m_simplestation, uint8_t m_id);
uint32_t m_channel_get_transfer_size(m_simplestation_state *m_simplestation, uint8_t m_id);
void m_channel_set_done(m_simplestation_state *m_simplestation, uint8_t m_id);
