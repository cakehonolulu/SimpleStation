#include <renderer/vulkan/vk_instance.h>

VkInstance vk_create_instance(vulcano_struct *vulcano_state, bool *vulkan_error)
{
    VkInstance ret = {0};
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


    VkInstanceCreateInfo creation_info = {0};
    creation_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    creation_info.pNext = NULL;
    creation_info.pApplicationInfo = &app_info;
    creation_info.enabledExtensionCount =  0;
    creation_info.ppEnabledExtensionNames = NULL;
    creation_info.enabledLayerCount = 0;
    creation_info.ppEnabledLayerNames = NULL;

    /* First Find the instance extensions */

    // Find how many extensions the instance will have using Vulkan API functions

    if (vkEnumerateInstanceExtensionProperties(NULL, &vulcano_state->vulkan_instance_extensions_count, NULL) != VK_SUCCESS)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to get Vulkan Extensions count, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    // Allocate space for the extensions information
    vulcano_state->vulkan_instance_extensions = malloc(sizeof(VkExtensionProperties) * vulcano_state->vulkan_instance_extensions_count);

    // Get the actual available extensions
    if (vkEnumerateInstanceExtensionProperties(NULL, &vulcano_state->vulkan_instance_extensions_count, vulcano_state->vulkan_instance_extensions) != VK_SUCCESS)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to obtain Vulkan Extension names, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    if (SDL_Vulkan_GetInstanceExtensions(vulcano_state->vulcano_window, &vulcano_state->sdl_instance_extension_count, NULL) != SDL_TRUE)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to obtain SDL Instance Extensions count, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    vulcano_state->sdl_instance_extensions = malloc(sizeof(char *) * vulcano_state->sdl_instance_extension_count);
    
    if (SDL_Vulkan_GetInstanceExtensions(vulcano_state->vulcano_window, &(vulcano_state->sdl_instance_extension_count), vulcano_state->sdl_instance_extensions) != SDL_TRUE)
    {
        printf(RED "[vulkan] vk_create_instance: Failed to obtain SDL Instance Extension names, exiting..." NORMAL "\n");
        *vulkan_error = true;
        goto vk_create_instance_end;
    }

    printf(BOLD MAGENTA "[vulkan] vk_create_instance: %d available sdl instance extensions..." NORMAL "\n", vulcano_state->sdl_instance_extension_count);

    for (size_t i = 0; i < vulcano_state->sdl_instance_extension_count; i++)
    {
        printf(MAGENTA "[vulkan] #%lu > %s" NORMAL "\n", i, vulcano_state->sdl_instance_extensions[i]);
    }


    printf(BOLD MAGENTA "[vulkan] vk_create_instance: %d available instance extensions..." NORMAL "\n", vulcano_state->vulkan_instance_extensions_count);

    for (size_t i = 0; i < vulcano_state->vulkan_instance_extensions_count; i++)
    {
        printf(MAGENTA "[vulkan] #%lu > %s (Ver. %d)" NORMAL "\n", i, vulcano_state->vulkan_instance_extensions[i].extensionName, vulcano_state->vulkan_instance_extensions[i].specVersion);
    }


    size_t cnt = 0, needed_layers = 0;
    bool portability_enumeration = false, phys_prop2 = false, khr_surface = false;

#if !defined (__APPLE__) && !defined (__MINGW64__)
    bool xlib_surface = false;
#endif
#ifdef __APPLE__
    bool khr_metal_surface = false;
#endif

#if !defined (__APPLE__) && defined (__MINGW64__)
    bool win32_surface = false;
#endif

    const char **chosen_instance_extensions = NULL;

    while (cnt < vulcano_state->vulkan_instance_extensions_count)
    {
        if (strcmp(vulcano_state->vulkan_instance_extensions[cnt].extensionName, "VK_KHR_surface") == 0)
        {
            khr_surface = true;
            needed_layers++;
        }
#if !defined (__APPLE__) && defined (__MINGW64__)
        if (strcmp(vulcano_state->vulkan_instance_extensions[cnt].extensionName, "VK_KHR_win32_surface") == 0)
        {
            win32_surface = true;
            needed_layers++;
        }
#endif
#if !defined (__APPLE__) && !defined (__MINGW64__)
        if (strcmp(vulcano_state->vulkan_instance_extensions[cnt].extensionName, "VK_KHR_xlib_surface") == 0)
        {
            xlib_surface = true;
            needed_layers++;
        }
#endif
#ifdef __APPLE__
        else if (strcmp(vulcano_state->vulkan_instance_extensions[cnt].extensionName, "VK_KHR_portability_enumeration") == 0)
        {
            portability_enumeration = true;
            needed_layers++;
        }
        else if (strcmp(vulcano_state->vulkan_instance_extensions[cnt].extensionName, "VK_KHR_get_physical_device_properties2") == 0)
        {
            phys_prop2 = true;
            needed_layers++;
        }
        else if (strcmp(vulcano_state->vulkan_instance_extensions[cnt].extensionName, "VK_EXT_metal_surface") == 0)
        {
            khr_metal_surface = true;
            needed_layers++;
        }
#endif

        cnt++;
    }

    cnt = 0;

    if (needed_layers > 0)
    {
        chosen_instance_extensions = malloc(sizeof(char *) * needed_layers);
    }

    if (khr_surface)
    {
        chosen_instance_extensions[cnt] = malloc(sizeof(char) * (strlen("VK_KHR_surface") + 1));
        chosen_instance_extensions[cnt] = "VK_KHR_surface";
        cnt++;

        printf(BOLD MAGENTA "[vulkan] vk_create_instance: Enabling VK_KHR_surface extension..." NORMAL "\n");
        creation_info.enabledExtensionCount++;
    }

#if !defined (__APPLE__) && defined (__MINGW64__)
    if (win32_surface)
    {
        chosen_instance_extensions[cnt] = malloc(sizeof(char) * (strlen("VK_KHR_win32_surface") + 1));
        chosen_instance_extensions[cnt] = "VK_KHR_win32_surface";
        cnt++;

        printf(BOLD MAGENTA "[vulkan] vk_create_instance: Enabling VK_KHR_win32_surface extension..." NORMAL "\n");
        creation_info.enabledExtensionCount++;
    }
#endif

#if !defined (__APPLE__) && !defined (__MINGW64__)
    if (xlib_surface)
    {
        chosen_instance_extensions[cnt] = malloc(sizeof(char) * (strlen("VK_KHR_xlib_surface") + 1));
        chosen_instance_extensions[cnt] = "VK_KHR_xlib_surface";
        cnt++;

        printf(BOLD MAGENTA "[vulkan] vk_create_instance: Enabling VK_KHR_xlib_surface extension..." NORMAL "\n");
        creation_info.enabledExtensionCount++;
    }
#endif

#ifdef __APPLE__
    if (portability_enumeration)
    {
        chosen_instance_extensions[cnt] = malloc(sizeof(char) * (strlen("VK_KHR_portability_enumeration") + 1));
        chosen_instance_extensions[cnt] = "VK_KHR_portability_enumeration";
        cnt++;

        printf(BOLD MAGENTA "[vulkan] vk_create_instance: Enabling VK_KHR_portability_enumeration (macOS Compatibility)..." NORMAL "\n");
        creation_info.enabledExtensionCount++;
    }

    if (phys_prop2)
    {
        chosen_instance_extensions[cnt] = malloc(sizeof(char) * (strlen("VK_KHR_get_physical_device_properties2") + 1));
        chosen_instance_extensions[cnt] = "VK_KHR_get_physical_device_properties2";
        cnt++;

        printf(BOLD MAGENTA "[vulkan] vk_create_instance: Enabling VK_KHR_get_physical_device_properties2 (macOS Compatibility)..." NORMAL "\n");
        creation_info.enabledExtensionCount++;
    }

    if (khr_metal_surface)
    {
        chosen_instance_extensions[cnt] = malloc(sizeof(char) * (strlen("VK_EXT_metal_surface") + 1));
        chosen_instance_extensions[cnt] = "VK_EXT_metal_surface";
        cnt++;

        printf(BOLD MAGENTA "[vulkan] vk_create_instance: Enabling VK_EXT_metal_surface (macOS Compatibility)..." NORMAL "\n");
        creation_info.enabledExtensionCount++;
    }
#endif

    creation_info.ppEnabledExtensionNames = chosen_instance_extensions;

    cnt = 0;
    needed_layers = 0;

    /* Then the layer extensions */

    vkEnumerateInstanceLayerProperties(&vulcano_state->vulkan_layer_extensions_count, NULL);

    vulcano_state->vulkan_layer_extensions = malloc(sizeof(VkLayerProperties) * vulcano_state->vulkan_layer_extensions_count);

    vkEnumerateInstanceLayerProperties(&vulcano_state->vulkan_layer_extensions_count, vulcano_state->vulkan_layer_extensions);

    printf(BOLD BLUE "[vulkan] vk_create_instance: %d available layer extensions..." NORMAL "\n", vulcano_state->vulkan_layer_extensions_count);
    
    for (size_t x = 0; x < vulcano_state->vulkan_layer_extensions_count; x++)    
    {
        printf(BLUE "[vulkan] #%lu > %s" NORMAL "\n", x, vulcano_state->vulkan_layer_extensions[x].layerName);
    }

    bool valid_layers = false;

    char khronos_valid[] = "VK_LAYER_KHRONOS_validation";

    const char **chosen_layers = NULL;

    // Check if we have Vulkan Validation Layer Support
    while (cnt < vulcano_state->vulkan_layer_extensions_count)
    {
        if (strcmp(vulcano_state->vulkan_layer_extensions[cnt].layerName, "VK_LAYER_KHRONOS_validation") == 0)
        {
            valid_layers = true;
            needed_layers++;
        }

        cnt++;
    }

    cnt = 0;

    if (needed_layers > 0)
    {
        chosen_layers = malloc(sizeof(char *) * needed_layers);
    }

    if (valid_layers)
    {
        chosen_layers[cnt] = malloc(sizeof(char) * (strlen(khronos_valid) + 1));
        chosen_layers[cnt] = khronos_valid;
        cnt++;

        printf(BOLD BLUE "[vulkan] vk_create_instance: Enabling Vulkan Validation Layers..." NORMAL "\n");
        creation_info.enabledLayerCount++;
    }

    creation_info.ppEnabledLayerNames = chosen_layers;

#ifdef __APPLE__
    creation_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

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
