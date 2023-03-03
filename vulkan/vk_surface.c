#include <vk_surface.h>

void vk_surface_prepare(vulcano_struct *vulcano_state)
{
	bool compatible_format = false;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*vulcano_state->phys_dev, vulcano_state->surface, &vulcano_state->vk_surface_capabilities);
	
    uint32_t vk_surface_fmt_num = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*vulcano_state->phys_dev, vulcano_state->surface, &vk_surface_fmt_num, NULL);
	
    VkSurfaceFormatKHR *vk_surface_fmts = malloc(sizeof(VkSurfaceFormatKHR) * vk_surface_fmt_num);
	vkGetPhysicalDeviceSurfaceFormatsKHR(*vulcano_state->phys_dev, vulcano_state->surface, &vk_surface_fmt_num, vk_surface_fmts);
	
	printf(BOLD GREEN "[vulkan] vk_surface_prepare: Available Surface Formats:" NORMAL "\n");

	for (uint32_t i = 0; i < vk_surface_fmt_num; i++)
	{
		if (vk_surface_fmts[i].format == VK_FORMAT_B8G8R8A8_UNORM) compatible_format = true;

		printf(GREEN "- %d" NORMAL "\n", vk_surface_fmts[i].format);
	}

    vulcano_state->vk_surface_format = vk_surface_fmts[0];

	if (compatible_format)
	{
		printf(BOLD GREEN "[vulkan] vk_surface_prepare: Setting compatible surface format!" NORMAL "\n");
		vulcano_state->vk_surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
	}

	free(vk_surface_fmts);

    uint32_t vk_surface_present_mode_num = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(*vulcano_state->phys_dev, vulcano_state->surface, &vk_surface_present_mode_num, NULL);
	
    VkPresentModeKHR *vk_surface_present_modes = malloc(sizeof(VkPresentModeKHR) * vk_surface_present_mode_num);
	vkGetPhysicalDeviceSurfacePresentModesKHR(*vulcano_state->phys_dev, vulcano_state->surface, &vk_surface_present_mode_num, vk_surface_present_modes);

	vulcano_state->vk_surface_chosen_present = VK_PRESENT_MODE_FIFO_KHR;

	for (uint32_t i = 0; i < vk_surface_present_mode_num; i++)
    {
		if (vk_surface_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
			vulcano_state->vk_surface_chosen_present = VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}

	free(vk_surface_present_modes);
}
