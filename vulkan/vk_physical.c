#include <vk_physical.h>

int vk_pick_physical_device(vulcano_struct *vulcano_state, bool *vulkan_error)
{
    unsigned int physical_device_cnt = 0, preferred_phys_device = 0, phys_device_idx = 0, current_phys_dev_idx = 0;
    int phys_dev = -1;

    bool finding_suitable = true;

    // Enumerate physical devices
    vkEnumeratePhysicalDevices(vulcano_state->instance, &physical_device_cnt, NULL);

    if (physical_device_cnt == 0)
    {
        printf(RED "[vulkan] vk_pick_physical_device: Vulkan-compatible compute devices not found, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_pick_physical_device_end;
    }
    else
    {
        printf(YELLOW "[vulkan] vk_pick_physical_device: Found %d compute devices..." NORMAL "\n", physical_device_cnt);
    }

    phys_dev_t *phys_devices = malloc(sizeof(phys_dev_t) * physical_device_cnt);

    vulcano_state->physical_devices = malloc (sizeof(VkPhysicalDevice) * physical_device_cnt);

    vkEnumeratePhysicalDevices(vulcano_state->instance, &physical_device_cnt, vulcano_state->physical_devices);

    VkPhysicalDeviceProperties *physical_devices_props = malloc(sizeof(VkPhysicalDeviceProperties) * physical_device_cnt);

    VkPhysicalDeviceMemoryProperties *physical_device_memory_props = malloc(sizeof(VkPhysicalDeviceMemoryProperties) * physical_device_cnt);

    for (size_t i = 0; i < physical_device_cnt; i++)
    {
        vkGetPhysicalDeviceProperties(vulcano_state->physical_devices[i], &physical_devices_props[i]);
		vkGetPhysicalDeviceMemoryProperties(vulcano_state->physical_devices[i], &physical_device_memory_props[i]);

        if (physical_devices_props[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
			printf(BOLD GREEN "[vulkan] vk_pick_physical_device: Found dGPU!" NORMAL "\n");
            printf(BOLD GREEN "         ┌─ Name" NORMAL "\n");
            printf(BOLD GREEN "         │  · %s" NORMAL "\n", physical_devices_props[i].deviceName);
            printf(BOLD GREEN "         └─ Physical ID" NORMAL "\n");
            printf(BOLD GREEN "            · %ld" NORMAL "\n", i);
            phys_devices[phys_device_idx].id = i;
            phys_devices[phys_device_idx].type = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            phys_device_idx++;
		}
        else if (physical_devices_props[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            printf(BOLD GREEN "[vulkan] vk_pick_physical_device: Found iGPU!" NORMAL "\n");
            printf(BOLD GREEN "         ┌─ Name" NORMAL "\n");
            printf(BOLD GREEN "         │  · %s" NORMAL "\n", physical_devices_props[i].deviceName);
            printf(BOLD GREEN "         └─ Physical ID" NORMAL "\n");
            printf(BOLD GREEN "            · %ld" NORMAL "\n", i);
            phys_devices[phys_device_idx].id = i;
            phys_devices[phys_device_idx].type = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
            phys_device_idx++;
        }
        else
        {
            printf(YELLOW "[vulkan] vk_pick_physical_device: Unsupported compute device %d" NORMAL "\n", physical_devices_props[i].deviceType);
        }
    }

    while (finding_suitable && current_phys_dev_idx < phys_device_idx)
    {
        // FIXME?: This doesn't account for more-than-1 dGPU/iGPU system (Don't know of any but just in case)
        if (phys_devices[current_phys_dev_idx].type == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            printf(BOLD GREEN "[vulkan] vk_pick_physical_device: Using discrete GPU!" NORMAL "\n");
            phys_dev = phys_devices[current_phys_dev_idx].id;
            finding_suitable = false;
        }
        else if (finding_suitable && phys_devices[current_phys_dev_idx].type == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            printf(BOLD YELLOW "[vulkan] vk_pick_physical_device: Using integrated GPU fallback, performance might be sub-par..." NORMAL "\n");
            phys_dev = phys_devices[current_phys_dev_idx].id;
        }

        current_phys_dev_idx++;
    }

vk_pick_physical_device_end:
    if (phys_devices)
        free(phys_devices);

    if (physical_devices_props)
        free(physical_devices_props);

    if (physical_device_memory_props)
        free(physical_device_memory_props);

    return phys_dev;
}
