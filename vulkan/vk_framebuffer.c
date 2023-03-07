#include <vk_framebuffer.h>

void vk_framebuffer_prepare(vulcano_struct *vulcano_state)
{
    VkAttachmentDescription attachmentDescription = {
		0,
		vulcano_state->vk_surface_format.format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference attachmentReference = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpassDescription = {
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		NULL,
		1,
		&attachmentReference,
		NULL,
		NULL,
		0,
		NULL
	};

	VkSubpassDependency subpassDependency = {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		0
	};

	VkRenderPassCreateInfo renderPassCreateInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		NULL,
		0,
		1,
		&attachmentDescription,
		1,
		&subpassDescription,
		1,
		&subpassDependency
	};

	vkCreateRenderPass(vulcano_state->device, &renderPassCreateInfo, NULL, &vulcano_state->vk_render_pass);
}

void vk_framebuffer_create(vulcano_struct *vulcano_state)
{
    VkFramebufferCreateInfo *vk_framebuffer_create_info = malloc(sizeof(VkFramebufferCreateInfo) * vulcano_state->vk_swapchain_img_num);
	vulcano_state->vk_framebuffer = malloc(sizeof(VkFramebuffer) * vulcano_state->vk_swapchain_img_num);

	for (uint32_t i = 0; i < vulcano_state->vk_swapchain_img_num; i++)
    {
		vk_framebuffer_create_info[i].sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		vk_framebuffer_create_info[i].pNext = NULL;
		vk_framebuffer_create_info[i].flags = 0;
		vk_framebuffer_create_info[i].renderPass = vulcano_state->vk_render_pass;
		vk_framebuffer_create_info[i].attachmentCount = 1;
		vk_framebuffer_create_info[i].pAttachments = &vulcano_state->vk_image_view[i];
		vk_framebuffer_create_info[i].width = vulcano_state->vk_swapchain_chosen.width;
		vk_framebuffer_create_info[i].height = vulcano_state->vk_swapchain_chosen.height;
		vk_framebuffer_create_info[i].layers = 1;

		vkCreateFramebuffer(vulcano_state->device, &vk_framebuffer_create_info[i], NULL, &vulcano_state->vk_framebuffer[i]);
	}

	free(vk_framebuffer_create_info);
}
