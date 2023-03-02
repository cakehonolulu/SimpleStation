#pragma once

#include <vulcano.h>

/* Function Definitions */
void vk_create_pipeline(vulcano_struct *vulcano_state);
VkPipelineShaderStageCreateInfo vk_config_vshader_info(vulcano_struct *vulcano_state, const char *shader_entrypoint);
VkPipelineShaderStageCreateInfo vk_config_fshader_info(vulcano_struct *vulcano_state, const char *shader_entrypoint);
VkPipelineVertexInputStateCreateInfo vk_config_input_state(vulcano_struct *vulcano_state);
VkPipelineInputAssemblyStateCreateInfo vk_config_input_assembly_state();
VkViewport vk_viewport_config(vulcano_struct *vulcano_state);
VkRect2D vk_scissor_config(vulcano_struct *vulcano_state, uint32_t left, uint32_t right, uint32_t top, uint32_t bottom);
VkPipelineViewportStateCreateInfo vk_config_viewport_state_info(VkViewport *viewport, VkRect2D *scissor);
VkPipelineRasterizationStateCreateInfo vk_config_raster_state_info();
VkPipelineMultisampleStateCreateInfo vk_config_multisample_state_info();
VkPipelineColorBlendAttachmentState vk_config_color_blend_attatchment_state();
VkPipelineColorBlendStateCreateInfo vk_config_color_blend_info_state(VkPipelineColorBlendAttachmentState *attatchment);
