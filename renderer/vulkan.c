#ifdef VULKAN_SUPPORT

#include <renderer/vulkan.h>
#include <simplestation.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void init_vulkan(m_simplestation_state *m_simplestation)
{
    if (glfwVulkanSupported())
    {
        printf("Vulkan is supported, yay!\n");
        exit(1);
    }
    else
    {
        printf("Vulkan isn't supported :(\n");
        exit(1);
    }
}

#endif
