#pragma once

#include <vulcano.h>

/* Structures */
typedef struct {
    size_t id;
    VkPhysicalDeviceType type;
} phys_dev_t;

/* Function Definitions */
int vk_pick_physical_device(vulcano_struct *vulcano_state, bool *vulkan_error);
