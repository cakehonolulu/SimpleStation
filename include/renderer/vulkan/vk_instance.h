#pragma once

#include <renderer/vulkan/vulcano.h>

/* Function Definitions */
VkInstance vk_create_instance(vulcano_struct *vulcano_state, bool *vulkan_error);
