#pragma once

#include <renderer/vulkan/vulcano.h>

/* Function Definitions */
VkDevice vk_create_device(vulcano_struct *vulcano_state, uint32_t queue_family_number);
void vk_list_device_ext(vulcano_struct *vulcano_state);
