#include "instance.h"

CInstance::CInstance(const std::vector<const char*> requiredValidationLayers){
    debugger = new CDebugger("../logs/instanceLog.txt");

    HERE_I_AM("CInstance Constructor");

    VkResult result = VK_SUCCESS;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //First make sure required layer(s) are available
    if (enableValidationLayers) {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr); // see what layers are actually available:

        std::vector<VkLayerProperties> availableLayers(layerCount);
        result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        REPORT("vkEnumerateInstanceLayerProperties");

        debugger->writeMSG("Available layers:\n");
        if(debugger->getVerbose()){
            for (const auto& layerProperties : availableLayers) {
                fprintf(debugger->FpDebug, "0x%08x  %5d  '%s'  '%s'\n",
                    layerProperties.specVersion,
                    layerProperties.implementationVersion,
                    layerProperties.layerName,
                    layerProperties.description);
            }
            debugger->writeMSG("\n");
        }

        for (const char* layerName : requiredValidationLayers) {// to find out validation layer in available layers
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    debugger->writeMSG("validation layers available!(%s)\n\n", layerName);
                    break;
                }
            }
        }

        if (result != VK_SUCCESS) throw std::runtime_error("validation layers requested, but not available!");
    }

    //Second set required extension(s)
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties((char *)nullptr, &extensionCount, (VkExtensionProperties *)nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    result = vkEnumerateInstanceExtensionProperties((char *)nullptr, &extensionCount, availableExtensions.data());
    REPORT("vkEnumerateInstanceExtensionProperties");
    debugger->writeMSG("Available extensions:\n");
    if(debugger->getVerbose()){
        for (const auto& extensionsProperties : availableExtensions) {
            fprintf(debugger->FpDebug, "0x%08x  '%s'\n",
                extensionsProperties.specVersion,
                extensionsProperties.extensionName);
        }
        debugger->writeMSG("\n");
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = requiredValidationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    debugger->writeMSG("Required extensions:\n");
    if(debugger->getVerbose()){
        for (const auto& extensionsProperties : extensions) {
            fprintf(debugger->FpDebug, "'%s'\n",
                extensionsProperties);
        }
        debugger->writeMSG("\n");
    }

    //Third create instance
    result = vkCreateInstance(&createInfo, nullptr, &handle);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create instance!");

    REPORT("vkCreateInstance");

    Init02SetupDebugMessenger();
}

CInstance::~CInstance(){
    if (!debugger) delete debugger;
    //if (handle != VK_NULL_HANDLE) vkDestroyInstance(handle, nullptr);
}


void CInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

std::vector<const char*> CInstance::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkResult CInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void CInstance::Init02SetupDebugMessenger() {
    HERE_I_AM("Init02CreateDebugMessenger");

    VkResult result = VK_SUCCESS;

    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    result = CreateDebugUtilsMessengerEXT(handle, &createInfo, nullptr, &debugMessenger);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to set up debug messenger!");

    REPORT("CreateDebugUtilsMessengerEXT");
}

void CInstance::findAllPhysicalDevices(){
    HERE_I_AM("findAllPhysicalDevices");

    VkResult result = VK_SUCCESS;

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, (VkPhysicalDevice *)nullptr);
    
    if (result != VK_SUCCESS || deviceCount == 0) {
        debugger->writeMSG("Could not count the physical devices\n");
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    debugger->writeMSG("%d physical devices found.\n", deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, OUT devices.data());
    REPORT("vkEnumeratePhysicalDevices");

    if (result != VK_SUCCESS) {
        debugger->writeMSG("Could not enumerate the %d physical devices\n", deviceCount);
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    if(debugger->getVerbose()){
        for (unsigned int i = 0; i < deviceCount; i++) 
            displayPhysicalDevices(devices[i], i);
        debugger->writeMSG("\n");
    }

    for (auto &physical_device : devices)
        //gpus.push_back(std::make_unique<CPhysicalDevice>(*this, physical_device));
        physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));

}

void CInstance::displayPhysicalDevices(VkPhysicalDevice physicalDevice, int index){
    VkPhysicalDeviceProperties	PhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(IN physicalDevice, OUT &PhysicalDeviceProperties);

    fprintf(debugger->FpDebug, "\nDevice %2d:\n", index);
    fprintf(debugger->FpDebug, "\tAPI version: %d\n", PhysicalDeviceProperties.apiVersion);
    fprintf(debugger->FpDebug, "\tDriver version: %d\n", PhysicalDeviceProperties.apiVersion);
    fprintf(debugger->FpDebug, "\tVendor ID: 0x%04x\n", PhysicalDeviceProperties.vendorID);
    fprintf(debugger->FpDebug, "\tDevice ID: 0x%04x\n", PhysicalDeviceProperties.deviceID);
    fprintf(debugger->FpDebug, "\tPhysical Device Type: %d =", PhysicalDeviceProperties.deviceType);
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)	fprintf(debugger->FpDebug, " (Discrete GPU)\n");
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)	fprintf(debugger->FpDebug, " (Integrated GPU)\n");
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)	fprintf(debugger->FpDebug, " (Virtual GPU)\n");
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)		fprintf(debugger->FpDebug, " (CPU)\n");
    fprintf(debugger->FpDebug, "\tDevice Name: %s\n", PhysicalDeviceProperties.deviceName);
    fprintf(debugger->FpDebug, "\tPipeline Cache Size: %d\n", PhysicalDeviceProperties.pipelineCacheUUID[0]);


    //Display PhysicalDevice Features
    VkPhysicalDeviceFeatures	PhysicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(IN physicalDevice, OUT &PhysicalDeviceFeatures);

    fprintf(debugger->FpDebug, "\n\tPhysical Device Features:\n");
    fprintf(debugger->FpDebug, "\tgeometryShader = %2d\n", PhysicalDeviceFeatures.geometryShader);
    fprintf(debugger->FpDebug, "\ttessellationShader = %2d\n", PhysicalDeviceFeatures.tessellationShader);
    fprintf(debugger->FpDebug, "\tmultiDrawIndirect = %2d\n", PhysicalDeviceFeatures.multiDrawIndirect);
    fprintf(debugger->FpDebug, "\twideLines = %2d\n", PhysicalDeviceFeatures.wideLines);
    fprintf(debugger->FpDebug, "\tlargePoints = %2d\n", PhysicalDeviceFeatures.largePoints);
    fprintf(debugger->FpDebug, "\tmultiViewport = %2d\n", PhysicalDeviceFeatures.multiViewport);
    fprintf(debugger->FpDebug, "\tocclusionQueryPrecise = %2d\n", PhysicalDeviceFeatures.occlusionQueryPrecise);
    fprintf(debugger->FpDebug, "\tpipelineStatisticsQuery = %2d\n", PhysicalDeviceFeatures.pipelineStatisticsQuery);
    fprintf(debugger->FpDebug, "\tshaderFloat64 = %2d\n", PhysicalDeviceFeatures.shaderFloat64);
    fprintf(debugger->FpDebug, "\tshaderInt64 = %2d\n", PhysicalDeviceFeatures.shaderInt64);
    fprintf(debugger->FpDebug, "\tshaderInt16 = %2d\n", PhysicalDeviceFeatures.shaderInt16);

    VkFormatProperties					vfp;
    fprintf(debugger->FpDebug, "\n\tImage Formats Checked:\n");
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_R32G32B32A32_SFLOAT, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_R32G32B32A32_SFLOAT: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_R8G8B8A8_UNORM, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_R8G8B8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_B8G8R8A8_UNORM, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_B8G8R8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_B8G8R8A8_SRGB, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_B8G8R8A8_SRGB: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);

    VkPhysicalDeviceMemoryProperties			vpdmp;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, OUT &vpdmp);
    fprintf(debugger->FpDebug, "\n\t%d Memory Types:\n", vpdmp.memoryTypeCount);
    for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
        VkMemoryType vmt = vpdmp.memoryTypes[i];
        VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
        fprintf(debugger->FpDebug, "\tMemory %2d: ", i);
        if ((vmpf & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)	fprintf(debugger->FpDebug, " DeviceLocal");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)	fprintf(debugger->FpDebug, " HostVisible");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0)	fprintf(debugger->FpDebug, " HostCoherent");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0)	fprintf(debugger->FpDebug, " HostCached");
        if ((vmpf & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0)	fprintf(debugger->FpDebug, " LazilyAllocated");
        fprintf(debugger->FpDebug, "\n");
    }

    fprintf(debugger->FpDebug, "\n\t%d Memory Heaps:\n", vpdmp.memoryHeapCount);
    for (unsigned int i = 0; i < vpdmp.memoryHeapCount; i++) {
        fprintf(debugger->FpDebug, "\tHeap %d: ", i);
        VkMemoryHeap vmh = vpdmp.memoryHeaps[i];
        fprintf(debugger->FpDebug, " size = 0x%08lx", (unsigned long int)vmh.size);
        if ((vmh.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)	fprintf(debugger->FpDebug, " DeviceLocal");
        if ((vmh.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT) != 0)	fprintf(debugger->FpDebug, " MultiInstance");
        fprintf(debugger->FpDebug, "\n");
    }
    fprintf(debugger->FpDebug, "\n");

    // see what device layers are available:
    uint32_t layerCount;
    vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, (VkLayerProperties *)nullptr);
    VkLayerProperties * deviceLayers = new VkLayerProperties[layerCount];
    VkResult result = vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, deviceLayers);
    REPORT("\tvkEnumerateDeviceLayerProperties");
    fprintf(debugger->FpDebug, "\t%d physical device layers enumerated:\n", layerCount);
    for (unsigned int i = 0; i < layerCount; i++) {
        fprintf(debugger->FpDebug, "\t0x%08x  %2d  '%s'  '%s'\n",
            deviceLayers[i].specVersion,
            deviceLayers[i].implementationVersion,
            deviceLayers[i].layerName,
            deviceLayers[i].description);

        // see what device extensions are available:
        //show layer related extension
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, deviceLayers[i].layerName, &extensionCount, (VkExtensionProperties *)nullptr);
        VkExtensionProperties * deviceExtensions = new VkExtensionProperties[extensionCount];
        result = vkEnumerateDeviceExtensionProperties(physicalDevice, deviceLayers[i].layerName, &extensionCount, deviceExtensions);
        REPORT("\n\tvkEnumerateDeviceExtensionProperties");

        fprintf(debugger->FpDebug, "\t%d device extensions enumerated for '%s':\n", extensionCount, deviceLayers[i].layerName);
        for (unsigned int ii = 0; ii < extensionCount; ii++) {
            fprintf(debugger->FpDebug, "\t0x%08x  '%s'\n",
                deviceExtensions[ii].specVersion,
                deviceExtensions[ii].extensionName);
        }
        fprintf(debugger->FpDebug, "\n");
    }
    delete[] deviceLayers;
    debugger->flush();
}

bool CInstance::pickSuitablePhysicalDevice(VkSurfaceKHR surface, const std::vector<const char*>  requireDeviceExtensions, VkQueueFlagBits requiredQueueFamilies){
    HERE_I_AM("pickSuitablePhysicalDevice");

    if(physicalDevices.empty()){
        debugger->writeMSG("Error: No physical devices!\n");
        return false;
    }

    for ( auto& phy_device : physicalDevices) {
        //测试Queue Families
        QueueFamilyIndices indices = phy_device->findQueueFamilies(surface);

        //测试Extension
        bool extensionsSupported = phy_device->checkDeviceExtensionSupport(requireDeviceExtensions);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            //测试SwapChain
            SwapChainSupportDetails swapChainSupport = phy_device->querySwapChainSupport(surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        if (indices.isComplete() && extensionsSupported && swapChainAdequate) {
            if(requiredQueueFamilies & VK_QUEUE_GRAPHICS_BIT) {
                debugger->writeMSG("Require VK_QUEUE_GRAPHICS_BIT\n"); debugger->flush();
                if(!indices.graphicsFamily.has_value()) return false;
                debugger->writeMSG("Picked physical device index: %d\n", indices.graphicsFamily.value());debugger->flush();
            }
            if(requiredQueueFamilies & VK_QUEUE_COMPUTE_BIT) {
                debugger->writeMSG("Require VK_QUEUE_COMPUTE_BIT\n");debugger->flush();
                if(!indices.computeFamily.has_value()) return false;
                debugger->writeMSG("Picked physical device index: %d\n", indices.computeFamily.value());debugger->flush();

            }
            if((requiredQueueFamilies & VK_QUEUE_COMPUTE_BIT) & VK_QUEUE_GRAPHICS_BIT){
                debugger->writeMSG("Require VK_QUEUE_COMPUTE_BIT & VK_QUEUE_GRAPHICS_BIT\n");debugger->flush();
                if(!indices.graphicsAndComputeFamily.has_value()) return false;
                debugger->writeMSG("Picked physical device index: %d\n", indices.graphicsAndComputeFamily.value());debugger->flush();
            }

            pickedPhysicalDevice = &phy_device;
            return true;
        }
    }

    //if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
    //}
    return false;
}

    
