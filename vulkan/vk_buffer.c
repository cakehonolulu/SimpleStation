#include <vk_buffer.h>

Vertex vertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};


void vk_buffer_create(vulcano_struct *vulcano_state)
{
    VkDeviceSize bufferSize = 8192;

    createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vulcano_state->vertexBuffer, &vulcano_state->vertexBufferMemory, vulcano_state);
    void* data;
    vkMapMemory(vulcano_state->device, vulcano_state->vertexBufferMemory, 0, 8192, 0, &data);
        memcpy(data, vertices, (size_t) sizeof(vertices));
    vkUnmapMemory(vulcano_state->device, vulcano_state->vertexBufferMemory);
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, vulcano_struct *vulcano_state)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(*vulcano_state->phys_dev, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    printf("[vulcano] failed to find memory!");
    exit(1);
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory, vulcano_struct *vulcano_state) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulcano_state->device, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        printf("failed to create buffer!");
        exit(1);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulcano_state->device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, vulcano_state);

    if (vkAllocateMemory(vulcano_state->device, &allocInfo, NULL, bufferMemory) != VK_SUCCESS) {
        printf("failed to allocate buffer memory!");
        exit(1);
    }

    vkBindBufferMemory(vulcano_state->device, *buffer, *bufferMemory, 0);
}
