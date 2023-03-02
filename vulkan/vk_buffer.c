#include <vk_buffer.h>

Vertex vertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

uint16_t indices[] = {
    0, 1, 2, 2, 3, 0
};


void vk_buffer_create(vulcano_struct *vulcano_state)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * sizeof(vertices);

    createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vulcano_state->vertexBuffer, &vulcano_state->vertexBufferMemory, vulcano_state);
    void* data;
    vkMapMemory(vulcano_state->device, vulcano_state->vertexBufferMemory, 0, sizeof(vertices[0]) * sizeof(vertices), 0, &data);
        memcpy(data, vertices, (size_t) sizeof(vertices));
    vkUnmapMemory(vulcano_state->device, vulcano_state->vertexBufferMemory);

    VkDeviceSize idxbufferSize = sizeof(indices[0]) * sizeof(indices);
    createBuffer(idxbufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vulcano_state->indexBuffer, &vulcano_state->indexBufferMemory, vulcano_state);

    vulcano_state->vertex_ct = sizeof(indices);

    void* data2;
    vkMapMemory(vulcano_state->device, vulcano_state->indexBufferMemory, 0, idxbufferSize, 0, &data2);
    memcpy(data2, indices, (size_t) sizeof(indices));
    vkUnmapMemory(vulcano_state->device, vulcano_state->indexBufferMemory);
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
