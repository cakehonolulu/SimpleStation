#pragma once

#include <vulcano.h>

/* Function Definitions */
size_t vk_graphics_queue_get_family_idx(vulcano_struct *vulcano_state, uint32_t prop_count);
size_t vk_graphics_queue_get_mode(vulcano_struct *vulcano_state, size_t queue_idx);
VkQueue vk_graphics_queue_get_draw(vulcano_struct *vulcano_state, uint32_t queue_family_idx);
VkQueue vk_graphics_queue_get_presenting(vulcano_struct *vulcano_state, uint32_t queue_family_idx, uint32_t queue_mode);
