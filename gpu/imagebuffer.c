#include <gpu/imagebuffer.h>

uint16_t imageBuffer_Read(m_simplestation_state *m_simplestation, uint32_t x, uint32_t y) {
	return m_simplestation->m_gpu_image_buffer->buffer[(y * 1024) + x];
}

void imageBuffer_IncrIndex(m_simplestation_state *m_simplestation) {
	m_simplestation->m_gpu_image_buffer->index += 1;

	m_simplestation->m_gpu_image_buffer->image_index += 1;

	if ((m_simplestation->m_gpu_image_buffer->image_index % m_simplestation->m_gpu_image_buffer->w) == 0) {
		m_simplestation->m_gpu_image_buffer->index = ((m_simplestation->m_gpu_image_buffer->y + (m_simplestation->m_gpu_image_buffer->image_index / m_simplestation->m_gpu_image_buffer->w)) * 1024) + m_simplestation->m_gpu_image_buffer->x;
	}
}

void imageBuffer_Store(m_simplestation_state *m_simplestation, uint32_t word)
{
	m_simplestation->m_gpu_image_buffer->buffer[m_simplestation->m_gpu_image_buffer->index] = (uint16_t)word;
	imageBuffer_IncrIndex(m_simplestation);

	m_simplestation->m_gpu_image_buffer->buffer[m_simplestation->m_gpu_image_buffer->index] = (uint16_t)(word >> 16);
	imageBuffer_IncrIndex(m_simplestation);
}

void imageBuffer_Reset(m_simplestation_state *m_simplestation, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	m_simplestation->m_gpu_image_buffer->x = x;
	m_simplestation->m_gpu_image_buffer->y = y;
	m_simplestation->m_gpu_image_buffer->w = w;
	m_simplestation->m_gpu_image_buffer->h = h;
	m_simplestation->m_gpu_image_buffer->image_index = 0;
	m_simplestation->m_gpu_image_buffer->index = x + y * 1024;
}

void imageBuffer_Create(m_simplestation_state *m_simplestation)
{
    m_simplestation->m_gpu_image_buffer = (m_psx_gpu_image_buffer_t *) malloc (sizeof(m_psx_gpu_image_buffer_t));
	memset(m_simplestation->m_gpu_image_buffer, 0, sizeof(m_psx_gpu_image_buffer_t));
	imageBuffer_Reset(m_simplestation, 0, 0, 0, 0);
}