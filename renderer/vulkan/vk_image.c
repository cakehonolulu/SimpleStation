#include <renderer/vulkan/vk_image.h>

void vk_image_prepare(vulcano_struct *vulcano_state)
{
	vkGetSwapchainImagesKHR(vulcano_state->device, vulcano_state->vk_swapchain, &vulcano_state->vk_swapchain_img_num, NULL);

    vulcano_state->vk_swapchain_imgs = malloc(sizeof(VkImage) * vulcano_state->vk_swapchain_img_num);

	vkGetSwapchainImagesKHR(vulcano_state->device, vulcano_state->vk_swapchain, &vulcano_state->vk_swapchain_img_num, vulcano_state->vk_swapchain_imgs);

    VkComponentMapping componentMapping = {
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY
	};

	VkImageSubresourceRange imageSubresourceRange = {
		VK_IMAGE_ASPECT_COLOR_BIT,
		0,
		1,
		0,
		vulcano_state->vk_image_array_layers
	};

	VkImageViewCreateInfo *vk_image_view_create_info = malloc(sizeof(VkImageViewCreateInfo) * vulcano_state->vk_swapchain_img_num);
	vulcano_state->vk_image_view = malloc(sizeof(VkImageView) * vulcano_state->vk_swapchain_img_num);

	for (uint32_t i = 0; i < vulcano_state->vk_swapchain_img_num; i++)
    {
		vk_image_view_create_info[i].sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vk_image_view_create_info[i].pNext = NULL;
		vk_image_view_create_info[i].flags = 0;
		vk_image_view_create_info[i].image = vulcano_state->vk_swapchain_imgs[i];
		vk_image_view_create_info[i].viewType = VK_IMAGE_VIEW_TYPE_2D;
		vk_image_view_create_info[i].format = vulcano_state->vk_surface_format.format;
		vk_image_view_create_info[i].components = componentMapping;
		vk_image_view_create_info[i].subresourceRange = imageSubresourceRange;

		vkCreateImageView(vulcano_state->device, &(vk_image_view_create_info[i]), NULL, &(vulcano_state->vk_image_view[i]));
	}

	free(vk_image_view_create_info);
}
