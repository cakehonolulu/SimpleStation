#include <vk_instance.h>

VkInstance vk_create_instance(vulcano_struct *vulcano_state, bool *vulkan_error)
{
    VkInstance ret = {0};
    size_t vulkan_extensions_size = 0;
    PFN_vkEnumerateInstanceVersion instance_version;
    uint32_t vulkan_version = VK_API_VERSION_1_0, maj_ver, min_ver, patch_ver;
   
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "Vulkan Demo";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = vulkan_version;

    instance_version = (PFN_vkEnumerateInstanceVersion) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");

    // Check if instance version exists
    if (instance_version != NULL)
    {
        vkEnumerateInstanceVersion(&vulkan_version);

        maj_ver = VK_VERSION_MAJOR(vulkan_version);
        min_ver = VK_VERSION_MINOR(vulkan_version);
        patch_ver = VK_VERSION_PATCH(vulkan_version);
    }
    else
    {
        maj_ver = 0;
        min_ver = 0;
        patch_ver = 0;
    }

    printf(BOLD GREEN "[vulkan] vk_create_instance: Detected Vulkan Version: %d.%d.%d" NORMAL "\n", maj_ver, min_ver, patch_ver);

    // Find how many extensions the instance will have using Vulkan API functions
    if (vkEnumerateInstanceExtensionProperties(NULL, &vulcano_state->vulkan_extensions_count, NULL) != VK_SUCCESS)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to get Vulkan Extensions count, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    vulkan_extensions_size = vulcano_state->vulkan_extensions_count;

    // Allocate space for the extensions information
    vulcano_state->vulkan_extensions = malloc(sizeof(VkExtensionProperties) * vulcano_state->vulkan_extensions_count);

    // Get the actual available extensions
    if (vkEnumerateInstanceExtensionProperties(NULL, &vulcano_state->vulkan_extensions_count, vulcano_state->vulkan_extensions) != VK_SUCCESS)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to obtain Vulkan Extension names, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    printf(BOLD YELLOW "[vulkan] vk_create_instance: %d available extensions..." NORMAL "\n", vulcano_state->vulkan_extensions_count);

#ifdef VULKAN_VERBOSE
    for (size_t i = 0; i < vulcano_state->vulkan_extensions_count; i++)
    {
        printf(YELLOW "[vulkan] #%lu > %s (Ver. %d)" NORMAL "\n", i, vulcano_state->vulkan_extensions[i].extensionName, vulcano_state->vulkan_extensions[i].specVersion);
    }
#endif

    if (SDL_Vulkan_GetInstanceExtensions(vulcano_state->vulcano_window, &vulcano_state->vulkan_extensions_count, NULL) != SDL_TRUE)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to obtain SDL Instance Extensions count, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    vulcano_state->vulkan_instance_extensions = malloc(sizeof(char *) * vulcano_state->vulkan_extensions_count);
    
    if (SDL_Vulkan_GetInstanceExtensions(vulcano_state->vulcano_window, &(vulcano_state->vulkan_extensions_count), vulcano_state->vulkan_instance_extensions) != SDL_TRUE)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to obtain SDL Instance Extension names, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    vkEnumerateInstanceLayerProperties(&vulcano_state->vulkan_layer_ext_cnt, NULL);

    vulcano_state->vulkan_layer_extensions = malloc(sizeof(VkLayerProperties) * vulcano_state->vulkan_layer_ext_cnt);

    vkEnumerateInstanceLayerProperties(&vulcano_state->vulkan_layer_ext_cnt, vulcano_state->vulkan_layer_extensions);

    printf(BOLD YELLOW "[vulkan] vk_create_instance: %d available layer extensions..." NORMAL "\n", vulcano_state->vulkan_layer_ext_cnt);
    
#ifdef VULKAN_VERBOSE
    for (size_t x = 0; x < vulcano_state->vulkan_layer_ext_cnt; x++)    
    {
        printf(BLUE "[vulkan] #%lu > %s" NORMAL "\n", x, vulcano_state->vulkan_layer_extensions[x].layerName);
    }
#endif

	const char *vulkan_layers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

    VkInstanceCreateInfo creation_info = {0};
    creation_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    creation_info.pNext = NULL;
    creation_info.pApplicationInfo = &app_info;
    creation_info.enabledLayerCount = 0;
    creation_info.ppEnabledLayerNames = NULL;
    creation_info.enabledExtensionCount =  vulcano_state->vulkan_extensions_count;
    creation_info.ppEnabledExtensionNames = vulcano_state->vulkan_instance_extensions;

    size_t cnt = 0;
    bool cond = true;

    // Check if we have Vulkan Validation Layer Support
    while (cnt < vulcano_state->vulkan_layer_ext_cnt && cond)
    {
        if (strcmp(vulcano_state->vulkan_layer_extensions[cnt].layerName, vulkan_layers[0]) == 0)
        {
            creation_info.enabledLayerCount = 1;
            creation_info.ppEnabledLayerNames = vulkan_layers;
            printf(BOLD YELLOW "[vulkan] vk_create_instance: Enabling Vulkan Validation Layers..." NORMAL "\n");
            cond = false;
        }

        cnt++;
    }

    free(vulcano_state->vulkan_extensions);

    switch (vkCreateInstance(&creation_info, NULL, &ret))
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            printf("vkCreateInstance: Error VK_ERROR_OUT_OF_HOST_MEMORY\n");
            *vulkan_error = true;
            break;

        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            printf("vkCreateInstance: Error VK_ERROR_OUT_OF_DEVICE_MEMORY\n");
            *vulkan_error = true;
            break;

        case VK_ERROR_INITIALIZATION_FAILED:
            printf("vkCreateInstance: Error VK_ERROR_INITIALIZATION_FAILED\n");
            *vulkan_error = true;
            break;

        case VK_ERROR_LAYER_NOT_PRESENT:
            printf("vkCreateInstance: Error VK_ERROR_LAYER_NOT_PRESENT\n");
            *vulkan_error = true;
            break;

        case VK_ERROR_EXTENSION_NOT_PRESENT:
            printf("vkCreateInstance: Error VK_ERROR_EXTENSION_NOT_PRESENT\n");
            *vulkan_error = true;
            break;

        case VK_ERROR_INCOMPATIBLE_DRIVER:
            printf("vkCreateInstance: Error VK_ERROR_INCOMPATIBLE_DRIVER\n");
            *vulkan_error = true;
            break;

        case VK_SUCCESS:
            break;
            
        default:
            break;
    }


vk_create_instance_end:
    return ret;
}
