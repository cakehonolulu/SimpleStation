#include <renderer/renderer.h>

uint8_t setup_renderer(m_simplestation_state *m_simplestation, renderstack_t *renderstack)
{
    uint8_t m_result;

    switch (m_simplestation->renderer)
    {
        case OPENGL:
            m_result = init_opengl_renderer(m_simplestation);
            break;

        case VULKAN:
            m_result = init_vulkan_renderer(m_simplestation);
            break;
                
        default:
            __builtin_unreachable();
            break;
    }

    if (m_result != 0)
    {
        printf(RED "[GPU] setup_renderer: Failed to initialize a renderer!\n" NORMAL);
    }
    else
    {
        switch (m_simplestation->renderer)
        {
            case OPENGL:
                setup_opengl_renderstack(m_simplestation, renderstack);
                break;

            case VULKAN:
                setup_vulkan_renderstack(m_simplestation, renderstack);
                break;
                    
            default:
                __builtin_unreachable();
                break;
        }
    }

    return m_result;
}

void setup_vulkan_renderstack(m_simplestation_state *m_simplestation, renderstack_t *renderstack)
{
    renderstack->gpu_clear_cache = &vulkan_gpu_clear_cache;
    renderstack->gpu_fill_rect = &vulkan_gpu_fill_rect;
    renderstack->gpu_draw_monochrome_opaque_quad = &vulkan_gpu_draw_monochrome_opaque_quad;
    renderstack->gpu_draw_texture_blend_opaque_quad = &vulkan_gpu_draw_texture_blend_opaque_quad;
    renderstack->gpu_draw_texture_raw_opaque_quad = &vulkan_gpu_draw_texture_raw_opaque_quad;
    renderstack->gpu_draw_shaded_opaque_triangle = &vulkan_gpu_draw_shaded_opaque_triangle;
    renderstack->gpu_draw_shaded_opaque_quad = &vulkan_gpu_draw_shaded_opaque_quad;
    renderstack->gpu_draw_texture_raw_variable_size_rect = &vulkan_gpu_draw_texture_raw_variable_size_rect;
    renderstack->gpu_draw_monochrome_opaque_1x1 = &vulkan_gpu_draw_monochrome_opaque_1x1;
    renderstack->gpu_image_draw = &vulkan_gpu_image_draw;
    renderstack->gpu_image_store = &vulkan_gpu_image_store;

    renderstack->draw = &vulkan_draw;
    renderstack->display = &vulkan_draw;
}

void setup_opengl_renderstack(m_simplestation_state *m_simplestation, renderstack_t *renderstack)
{
    renderstack->gpu_clear_cache = &m_gpu_clear_cache;
    renderstack->gpu_fill_rect = &m_gpu_fill_rect;
    renderstack->gpu_draw_monochrome_opaque_quad = &m_gpu_draw_monochrome_opaque_quad;
    renderstack->gpu_draw_texture_blend_opaque_quad = &m_gpu_draw_texture_blend_opaque_quad;
    renderstack->gpu_draw_texture_raw_opaque_quad = &m_gpu_draw_texture_raw_opaque_quad;
    renderstack->gpu_draw_shaded_opaque_triangle = &m_gpu_draw_shaded_opaque_triangle;
    renderstack->gpu_draw_shaded_opaque_quad = &m_gpu_draw_shaded_opaque_quad;
    renderstack->gpu_draw_texture_raw_variable_size_rect = &m_gpu_draw_texture_raw_variable_size_rect;
    renderstack->gpu_draw_monochrome_opaque_1x1 = &m_gpu_draw_monochrome_opaque_1x1;
    renderstack->gpu_image_draw = &m_gpu_image_draw;
    renderstack->gpu_image_store = &m_gpu_image_store;

    renderstack->draw = &draw;
    renderstack->display = &display;
}
