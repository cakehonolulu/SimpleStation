#pragma once

#include <renderer/vulkan/vulcano.h>

/* Function Definitions */
void vk_buffer_create(vulcano_struct *vulcano_state);
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, vulcano_struct *vulcano_state);
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory, vulcano_struct *vulcano_state);


