#include <vk_sync.h>

void vk_sync_setup(vulcano_struct *vulcano_state)
{
    vulcano_state->vk_max_frames = 2;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		NULL,
		0
	};

	vulcano_state->vk_wait_semaphore = malloc(sizeof(VkSemaphore) * vulcano_state->vk_max_frames);
    vulcano_state->vk_signal_semaphore = malloc(sizeof(VkSemaphore) * vulcano_state->vk_max_frames);

	for (size_t i = 0; i < vulcano_state->vk_max_frames; i++)
    {
		vkCreateSemaphore(vulcano_state->device, &semaphoreCreateInfo, NULL, &vulcano_state->vk_wait_semaphore[i]);
        vkCreateSemaphore(vulcano_state->device, &semaphoreCreateInfo, NULL, &vulcano_state->vk_signal_semaphore[i]);
	}

    VkFenceCreateInfo fenceCreateInfo = {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		NULL,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	vulcano_state->vk_front_fences = malloc(sizeof(VkFence) * vulcano_state->vk_max_frames);

	for (size_t i = 0; i < vulcano_state->vk_max_frames; i++)
    {
		vkCreateFence(vulcano_state->device, &fenceCreateInfo, NULL, &vulcano_state->vk_front_fences[i]);
	}

    vulcano_state->vk_back_fences = malloc(sizeof(VkFence) * vulcano_state->vk_max_frames);

    for(size_t i = 0; i < vulcano_state->vk_max_frames; i++)
    {
		vulcano_state->vk_back_fences[i] = NULL;
	}
}
