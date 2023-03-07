#include <renderer/vulkan/vk_graphics_queue.h>

size_t vk_graphics_queue_get_family_idx(vulcano_struct *vulcano_state, uint32_t prop_count)
{
	uint32_t candidate_count = 0;
	uint32_t *gfx_queue_family_idx = malloc(sizeof(uint32_t) * prop_count);
	uint32_t gfx_queue_family_sel_cnt = 0, gfx_queue_family_sel_idx = 0;

	for (uint32_t i = 0; i < prop_count; i++)
    {
		if ((vulcano_state->queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
			gfx_queue_family_idx[candidate_count] = i;
			candidate_count++;
		}
	}

	for (uint32_t i = 0; i < candidate_count; i++)
    {
		if (vulcano_state->queue_family_props[gfx_queue_family_idx[i]].queueCount > gfx_queue_family_sel_cnt)
        {
			gfx_queue_family_sel_cnt = vulcano_state->queue_family_props[gfx_queue_family_idx[i]].queueCount;
			gfx_queue_family_sel_idx = i;
		}
	}

	free(gfx_queue_family_idx);

	return gfx_queue_family_sel_idx;
}

size_t vk_graphics_queue_get_mode(vulcano_struct *vulcano_state, size_t queue_idx)
{
	// TODO: Double-check
    return (vulcano_state->queue_family_props[queue_idx].queueCount > 1 ? 1 : 
        (vulcano_state->queue_family_props[queue_idx].queueCount == 1 ? 0 : 2));
}

VkQueue vk_graphics_queue_get_draw(vulcano_struct *vulcano_state, uint32_t queue_family_idx)
{
	VkQueue queue = NULL;
	vkGetDeviceQueue(vulcano_state->device, queue_family_idx, 0, &queue);
	return queue;
}

VkQueue vk_graphics_queue_get_presenting(vulcano_struct *vulcano_state, uint32_t queue_family_idx, uint32_t queue_mode)
{
	VkQueue queue = NULL;

	if (queue_mode == 0)
    {
		vkGetDeviceQueue(vulcano_state->device, queue_family_idx, 0, &queue);
	}
    else if (queue_mode == 1)
    {
		vkGetDeviceQueue(vulcano_state->device, queue_family_idx, 1, &queue);
	}

	return queue;
}
