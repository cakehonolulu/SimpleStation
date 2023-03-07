#pragma once

#include <renderer/renderer.h>
#include <simplestation.h>
#include <stdlib.h>

/* Function Definitions */
uint8_t init_vulkan_renderer(m_simplestation_state *m_simplestation);
void vulkan_draw(m_simplestation_state *m_simplestation, bool clear_colour);
int vulkan_put_triangle(Vulkan_Vertex v1, Vulkan_Vertex v2, Vulkan_Vertex v3, m_simplestation_state *m_simplestation);
int vulkan_put_quad(Vulkan_Vertex v1, Vulkan_Vertex v2, Vulkan_Vertex v3, Vulkan_Vertex v4, m_simplestation_state *m_simplestation);

/* GP0 */
void vulkan_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_draw_texture_raw_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_draw_texture_raw_variable_size_rect(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_draw_monochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_image_draw(uint32_t m_value, m_simplestation_state *m_simplestation);
void vulkan_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation);
