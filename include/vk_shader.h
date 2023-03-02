#pragma once

#include <vulcano.h>

/* Function Definition */
char *getShaderCode(const char *fileName, uint32_t *pShaderSize);
void deleteShaderCode(char **ppShaderCode);
VkShaderModule createShaderModule(vulcano_struct *vulcan_state, char *pShaderCode, uint32_t shaderSize);
