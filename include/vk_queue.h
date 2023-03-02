#pragma once

#include <vulcano.h>

/* Function Definitions */
uint32_t vk_queue_get_prop_count_from_device(VkPhysicalDevice *device);
void vk_queue_get_props_from_device(vulcano_struct *vulcano_state, uint32_t queue_family_number);
