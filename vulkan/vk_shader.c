#include <vk_shader.h>

char *getShaderCode(const char *fileName, uint32_t *pShaderSize)
{
	if (pShaderSize == NULL)
    {
		return NULL;
	}

	FILE *fp = NULL;
	fp = fopen(fileName, "rb+");
    
	if (fp == NULL)
    {
		return NULL;
	}
    
	fseek(fp, 0l, SEEK_END);
	*pShaderSize = (uint32_t)ftell(fp);
	rewind(fp);

	char *shaderCode = (char *)malloc((*pShaderSize) * sizeof(char));
	fread(shaderCode, 1, *pShaderSize, fp);

	fclose(fp);
	return shaderCode;
}

void deleteShaderCode(char **ppShaderCode){
	free(*ppShaderCode);
}

VkShaderModule createShaderModule(vulcano_struct *vulcan_state, char *pShaderCode, uint32_t shaderSize)
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		NULL,
		0,
		shaderSize,
		(const uint32_t *)pShaderCode
	};

	VkShaderModule shaderModule;
	vkCreateShaderModule(vulcan_state->device, &shaderModuleCreateInfo, NULL, &shaderModule);
	return shaderModule;
}
