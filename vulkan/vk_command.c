#include <vk_command.h>
#include <gpu/renderer.h>

void vk_command_pool_init(vulcano_struct *vulcano_state)
{
	VkCommandPoolCreateInfo create_info = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		NULL,
		0,
		vulcano_state->vk_queue_family_idx
	};

	vkCreateCommandPool(vulcano_state->device, &create_info, NULL, &vulcano_state->vk_command_pool);

	VkCommandBufferAllocateInfo create_info_allocate = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		NULL,
		vulcano_state->vk_command_pool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		vulcano_state->vk_swapchain_img_num
	};

	vulcano_state->vk_command_buf = malloc(sizeof(VkCommandBuffer) * vulcano_state->vk_swapchain_img_num);
	vkAllocateCommandBuffers(vulcano_state->device, &create_info_allocate, vulcano_state->vk_command_buf);

    VkCommandBufferBeginInfo *vk_cmd_buf_begin_info = malloc(sizeof(VkCommandBufferBeginInfo) * vulcano_state->vk_swapchain_img_num);
	VkRenderPassBeginInfo *vk_render_pass_info = malloc(sizeof(VkRenderPassBeginInfo) * vulcano_state->vk_swapchain_img_num);

	VkRect2D renderArea = {
		{0, 0},
		{vulcano_state->vk_swapchain_chosen.width, vulcano_state->vk_swapchain_chosen.height}
	};

	VkClearValue clearValue = {0.0f, 0.0f, 0.0f, 0.0f};

	for (uint32_t i = 0; i < vulcano_state->vk_swapchain_img_num; i++)
    {
		vk_cmd_buf_begin_info[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vk_cmd_buf_begin_info[i].pNext = NULL;
		vk_cmd_buf_begin_info[i].flags = 0;
		vk_cmd_buf_begin_info[i].pInheritanceInfo = NULL;

		vk_render_pass_info[i].sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vk_render_pass_info[i].pNext = NULL;
		vk_render_pass_info[i].renderPass = vulcano_state->vk_render_pass;
		vk_render_pass_info[i].framebuffer = vulcano_state->vk_framebuffer[i];
		vk_render_pass_info[i].renderArea = renderArea;
		vk_render_pass_info[i].clearValueCount = 1;
		vk_render_pass_info[i].pClearValues = &clearValue;

		vkBeginCommandBuffer(vulcano_state->vk_command_buf[i], &vk_cmd_buf_begin_info[i]);
		vkCmdBeginRenderPass(vulcano_state->vk_command_buf[i], &vk_render_pass_info[i], VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(vulcano_state->vk_command_buf[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulcano_state->vk_pipeline);
		
		VkBuffer vertexBuffers[] = {vulcano_state->vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(vulcano_state->vk_command_buf[i], 0, 1, vertexBuffers, offsets);

		vkCmdDraw(vulcano_state->vk_command_buf[i], sizeof(Vertex) * VERTEX_BUFFER_LEN, 1, 0, 0);
		vkCmdEndRenderPass(vulcano_state->vk_command_buf[i]);
		vkEndCommandBuffer(vulcano_state->vk_command_buf[i]);
	}

	free(vk_render_pass_info);
	free(vk_cmd_buf_begin_info);
}
