#include <vk_swapchain.h>

void vk_swapchain_create(vulcano_struct *vulcano_state)
{
    int vk_fb_w = 0, vk_fb_h = 0;
	SDL_Vulkan_GetDrawableSize(vulcano_state->vulcano_window, &vk_fb_w, &vk_fb_h);

	if (vulcano_state->vk_surface_capabilities.currentExtent.width < vk_fb_w)
    {
		vulcano_state->vk_swapchain_chosen.width = vulcano_state->vk_surface_capabilities.currentExtent.width;
	}
    else
    {
		vulcano_state->vk_swapchain_chosen.width = vk_fb_w;
	}

	if (vulcano_state->vk_surface_capabilities.currentExtent.height < vk_fb_h)
    {
		vulcano_state->vk_swapchain_chosen.height = vulcano_state->vk_surface_capabilities.currentExtent.height;
	}
    else
    {
		vulcano_state->vk_swapchain_chosen.height = vk_fb_h;
	}

    uint32_t vk_image_layer_array = 1;

    VkSharingMode vk_image_sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
	uint32_t vk_image_fam_idx_cnt = 0, *vk_fam_idxs = NULL;
	uint32_t vk_image_fam_idx_queue[] = {0, 1};
	
    if (vulcano_state->vk_queue_mode == 1)
    {
		vk_image_sharing_mode = VK_SHARING_MODE_CONCURRENT;
		vk_image_fam_idx_cnt = 2;
		vk_fam_idxs = vk_image_fam_idx_queue;
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		NULL,
		0,
		vulcano_state->surface,
		vulcano_state->vk_surface_capabilities.minImageCount + 1,
		vulcano_state->vk_surface_format.format,
		vulcano_state->vk_surface_format.colorSpace,
		vulcano_state->vk_swapchain_chosen,
		vk_image_layer_array,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		vk_image_sharing_mode,
		vk_image_fam_idx_cnt,
		vk_fam_idxs,
		vulcano_state->vk_surface_capabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		vulcano_state->vk_surface_chosen_present,
		VK_TRUE,
		NULL
	};

	vkCreateSwapchainKHR(vulcano_state->device, &swapchainCreateInfo, NULL, &vulcano_state->vk_swapchain);
}
