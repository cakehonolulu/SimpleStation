#include <renderer/vulkan/vk_pipeline.h>

void vk_create_pipeline(vulcano_struct *vulcano_state)
{
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		NULL,
		0,
		0,
		NULL,
		0,
		NULL
	};

	vkCreatePipelineLayout(vulcano_state->device, &pipeline_layout_create_info, NULL, &vulcano_state->vk_pipeline_layout);

    char shader_entrypoint[] = "main";

	VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = {
		vk_config_vshader_info(vulcano_state, shader_entrypoint),
		vk_config_fshader_info(vulcano_state, shader_entrypoint)
	};
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = vk_config_input_state(vulcano_state);
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = vk_config_input_assembly_state();
	VkViewport viewport = vk_viewport_config(vulcano_state);
	VkRect2D scissor = vk_scissor_config(vulcano_state, 0, 0, 0, 0);
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = vk_config_viewport_state_info(&viewport, &scissor);
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = vk_config_raster_state_info();
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = vk_config_multisample_state_info();
	VkPipelineColorBlendAttachmentState color_blend_attachment_state = vk_config_color_blend_attatchment_state();
	VkPipelineColorBlendStateCreateInfo color_blend_info_state = vk_config_color_blend_info_state(&color_blend_attachment_state);

	VkGraphicsPipelineCreateInfo vk_graphic_pipeline_create_info = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		NULL,
		0,
		2,
		shaderStageCreateInfo,
		&vertexInputStateCreateInfo,
		&inputAssemblyStateCreateInfo,
		NULL,
		&viewportStateCreateInfo,
		&rasterizationStateCreateInfo,
		&multisampleStateCreateInfo,
		NULL,
		&color_blend_info_state,
		NULL,
		vulcano_state->vk_pipeline_layout,
		vulcano_state->vk_render_pass,
		0,
		NULL,
		-1
	};

	vkCreateGraphicsPipelines(vulcano_state->device, NULL, 1, &vk_graphic_pipeline_create_info, NULL, &vulcano_state->vk_pipeline);
}


VkPipelineShaderStageCreateInfo vk_config_vshader_info(vulcano_struct *vulcano_state, const char *shader_entrypoint)
{
	VkPipelineShaderStageCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		NULL,
		0,
		VK_SHADER_STAGE_VERTEX_BIT,
		vulcano_state->vertexShaderModule,
		shader_entrypoint,
		NULL
	};

	return create_info;
}

VkPipelineShaderStageCreateInfo vk_config_fshader_info(vulcano_struct *vulcano_state, const char *shader_entrypoint)
{
	VkPipelineShaderStageCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		NULL,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		vulcano_state->fragmentShaderModule,
		shader_entrypoint,
		NULL
	};

	return create_info;
}

VkPipelineVertexInputStateCreateInfo vk_config_input_state(vulcano_struct *vulcano_state)
{
	VkPipelineVertexInputStateCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		NULL,
		0,
		0,
		NULL,
		0,
		NULL
	};

	vulcano_state->vbindingdesc = malloc(sizeof(VkVertexInputBindingDescription));
	vulcano_state->vbindingdesc->binding = 0;
	vulcano_state->vbindingdesc->stride = sizeof(Vulkan_Vertex);
	vulcano_state->vbindingdesc->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	vulcano_state->vattribdesc = malloc(sizeof(VkVertexInputAttributeDescription) * 2);

	vulcano_state->vattribdesc[0].binding = 0;
	vulcano_state->vattribdesc[0].location = 0;
	vulcano_state->vattribdesc[0].format = VK_FORMAT_R32G32_SFLOAT;
	vulcano_state->vattribdesc[0].offset = offsetof(Vulkan_Vertex, pos);

	vulcano_state->vattribdesc[1].binding = 0;
	vulcano_state->vattribdesc[1].location = 1;
	vulcano_state->vattribdesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vulcano_state->vattribdesc[1].offset = offsetof(Vulkan_Vertex, color);

	create_info.vertexBindingDescriptionCount = 1;
	create_info.pVertexBindingDescriptions = vulcano_state->vbindingdesc;
	create_info.vertexAttributeDescriptionCount = 2;
	create_info.pVertexAttributeDescriptions = vulcano_state->vattribdesc;

	return create_info;
}

VkPipelineInputAssemblyStateCreateInfo vk_config_input_assembly_state()
{
	VkPipelineInputAssemblyStateCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		NULL,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};

	return create_info;
}

VkViewport vk_viewport_config(vulcano_struct *vulcano_state)
{
	VkViewport vk_viewport = {
		1.0f,
		1.0f,
		vulcano_state->vk_swapchain_chosen.width,
		vulcano_state->vk_swapchain_chosen.height,
		0.0f,
		1.0f
	};

	return vk_viewport;
}

VkRect2D vk_scissor_config(vulcano_struct *vulcano_state, uint32_t left, uint32_t right, uint32_t top, uint32_t bottom)
{
	if (left > vulcano_state->vk_swapchain_chosen.width)
    {
		left = vulcano_state->vk_swapchain_chosen.width;
	}
	if (right > vulcano_state->vk_swapchain_chosen.width)
    {
		right = vulcano_state->vk_swapchain_chosen.width;
	}
	if (top > vulcano_state->vk_swapchain_chosen.height)
    {
		top = vulcano_state->vk_swapchain_chosen.height;
	}
	if (bottom > vulcano_state->vk_swapchain_chosen.height)
    {
		bottom = vulcano_state->vk_swapchain_chosen.height;
	}

	VkOffset2D offset = {
		left,
		top
	};

	VkExtent2D extent = {
		vulcano_state->vk_swapchain_chosen.width - left - right,
		vulcano_state->vk_swapchain_chosen.height - top - bottom
	};

	VkRect2D scissor = {
		offset,
		extent
	};

	return scissor;
}

VkPipelineViewportStateCreateInfo vk_config_viewport_state_info(VkViewport *viewport, VkRect2D *scissor)
{
	VkPipelineViewportStateCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		NULL,
		0,
		1,
		viewport,
		1,
		scissor
	};

	return create_info;
}

VkPipelineRasterizationStateCreateInfo vk_config_raster_state_info()
{
	VkPipelineRasterizationStateCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		NULL,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_NONE,
		VK_FRONT_FACE_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	return create_info;
}

VkPipelineMultisampleStateCreateInfo vk_config_multisample_state_info()
{
	VkPipelineMultisampleStateCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		NULL,
		0,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FALSE,
		1.0f,
		NULL,
		VK_FALSE,
		VK_FALSE
	};

	return create_info;
}

VkPipelineColorBlendAttachmentState vk_config_color_blend_attatchment_state()
{
	VkPipelineColorBlendAttachmentState create_info = {
		VK_FALSE,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	return create_info;
}

VkPipelineColorBlendStateCreateInfo vk_config_color_blend_info_state(VkPipelineColorBlendAttachmentState *attatchment)
{
	VkPipelineColorBlendStateCreateInfo create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		NULL,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		1,
		attatchment,
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	return create_info;
}
