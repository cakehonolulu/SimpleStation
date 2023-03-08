#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <simplestation.h>
#include <renderer/opengl.h>

#include <SDL2/SDL.h>

#include <simplestation.h>

typedef void (*gp0)(uint32_t value, m_simplestation_state *m_simplestation);
typedef void (*draw_)(m_simplestation_state *m_simplestation, bool cond, bool cond2);
typedef void (*display_)(m_simplestation_state *m_simplestation);


typedef struct renderstack_s
{
    // GP0 0x01
	gp0 gpu_clear_cache;

    // GP0 0x02
    gp0 gpu_fill_rect;

    // GP0 0x28
    gp0 gpu_draw_monochrome_opaque_quad;
    
    // GP0 0x2C
    gp0 gpu_draw_texture_blend_opaque_quad;
    
    // GP0 0x2D
    gp0 gpu_draw_texture_raw_opaque_quad;
    
    // GP0 0x2F
    gp0 gpu_draw_texture_raw_semitransparent_quad;
    
    // GP0 0x30
    gp0 gpu_draw_shaded_opaque_triangle;
    
    // GP0 0x38
    gp0 gpu_draw_shaded_opaque_quad;
    
    // GP0 0x40
    gp0 gpu_draw_monochrome_opaque_line;
    
    // GP0 0x64
    gp0 gpu_draw_texture_blend_variable_size_rect;
    
    // GP0 0x65
    gp0 gpu_draw_texture_raw_variable_size_rect;
    
    // GP0 0x68
    gp0 gpu_draw_monochrome_opaque_1x1;
    
    // GP0 0xA0
    gp0 gpu_image_draw;
    
    // GP0 0xC0
    gp0 gpu_image_store;

    draw_ draw;
    display_ display;
} renderstack_t;

/* Function Definitions */
uint8_t setup_renderer(m_simplestation_state *m_simplestation, renderstack_t *renderstack);
void setup_vulkan_renderstack(m_simplestation_state *m_simplestation, renderstack_t *renderstack);
void setup_opengl_renderstack(m_simplestation_state *m_simplestation, renderstack_t *renderstack);

extern void vulkan_draw(m_simplestation_state *m_simplestation, bool clear_colour);

extern void draw(m_simplestation_state *m_simplestation, bool clear_colour, bool part);
extern void display(m_simplestation_state *m_simplestation);

/* Vulkan GP0 */
extern void vulkan_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_draw_texture_raw_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_draw_texture_raw_variable_size_rect(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_draw_monochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_image_draw(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void vulkan_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation);

/* OpenGL GP0 */
extern void m_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_monochrome_opaque_line(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_texture_raw_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_texture_raw_semitransparent_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_texture_raw_variable_size_rect(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_draw_monochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_image_draw(uint32_t m_value, m_simplestation_state *m_simplestation);
extern void m_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation);
