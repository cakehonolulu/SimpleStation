#include <vk_device.h>

VkDevice vk_create_device(vulcano_struct *vulcano_state, uint32_t queue_family_number)
{
    VkDevice device = { 0 };

    VkDeviceQueueCreateInfo *queue_create_info = malloc(sizeof(VkDeviceQueueCreateInfo) * queue_family_number);
	float **queue_priorities = malloc(sizeof(float *) * queue_family_number);

	for (uint32_t i = 0; i < queue_family_number; i++)
    {
		queue_priorities[i] = malloc(sizeof(float) * vulcano_state->queue_family_props[i].queueCount);

		for (uint32_t j = 0; j < vulcano_state->queue_family_props[i].queueCount; j++)
        {
			queue_priorities[i][j] = 1.0f;
		}

		queue_create_info[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info[i].pNext = NULL;
		queue_create_info[i].flags = 0;
		queue_create_info[i].queueFamilyIndex = i;
		queue_create_info[i].queueCount = vulcano_state->queue_family_props[i].queueCount;
		queue_create_info[i].pQueuePriorities = queue_priorities[i];
	}
  
    const char *vulkan_extensions[] = {
		"VK_KHR_swapchain"
	};

	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(*vulcano_state->phys_dev, &physicalDeviceFeatures);

	VkDeviceCreateInfo deviceCreateInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		NULL,
		0,
		queue_family_number,
		queue_create_info,
		0,
		NULL,
		1,
		vulkan_extensions,
		&physicalDeviceFeatures
	};

	vkCreateDevice(*vulcano_state->phys_dev, &deviceCreateInfo, NULL, &device);

	for (uint32_t i = 0; i < queue_family_number; i++)
    {
		free(queue_priorities[i]);
	}

    if (queue_priorities)
        free(queue_priorities);

    if (queue_create_info)
        free(queue_create_info);

    return device;
}

void vk_list_device_ext(vulcano_struct *vulcano_state)
{
    vkEnumerateDeviceExtensionProperties(*vulcano_state->phys_dev, NULL, &vulcano_state->vulkan_extensions_count, NULL);

    vulcano_state->vulkan_extensions = malloc(sizeof(VkExtensionProperties) * vulcano_state->vulkan_extensions_count);

    vkEnumerateDeviceExtensionProperties(*vulcano_state->phys_dev, NULL, &vulcano_state->vulkan_extensions_count, vulcano_state->vulkan_extensions);

    printf(BOLD GREEN "[vulkan] vk_list_device_ext: Listing %d available device extensions..." NORMAL "\n", vulcano_state->vulkan_extensions_count);

    for (size_t i = 0; i < vulcano_state->vulkan_extensions_count; i++)
    {
        printf(BOLD GREEN "[vulkan] #%lu > %s" NORMAL "\n", i, vulcano_state->vulkan_extensions[i].extensionName);
    }

    if (vulcano_state->vulkan_extensions)
        free(vulcano_state->vulkan_extensions);
}
