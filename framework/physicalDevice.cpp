#include "physicalDevice.h"

#include "instance.h"

CPhysicalDevice::CPhysicalDevice(VkPhysicalDevice physical_device) {
//CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device){
    //printf("physical\n");
    debugger = new CDebugger("physicalDeviceLog.txt");

    handle = physical_device;

    HERE_I_AM("Init03PhysicalDeviceAndGetQueueFamilyProperties");

    VkResult result = VK_SUCCESS;

    /*
    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(instance->getHandle(), OUT &deviceCount, (VkPhysicalDevice *)nullptr);
    REPORT("vkEnumeratePhysicalDevices - 1");

    if (result != VK_SUCCESS || deviceCount == 0) {
        debugger->writeMSG("Could not count the physical devices\n");
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    debugger->writeMSG("%d physical devices found.\nn", deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(instance->getHandle(), OUT &deviceCount, OUT devices.data());
    REPORT("vkEnumeratePhysicalDevices - 2");

    if (result != VK_SUCCESS) {
        debugger->writeMSG("Could not enumerate the %d physical devices\n", deviceCount);
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    for (unsigned int i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties	PhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(IN devices[i], OUT &PhysicalDeviceProperties);

        fprintf(debugger->FpDebug, "\nDevice %2d:\n", i);
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
    }*/

    //选择物理设备
    /*
    int whichDeviceSelected = -1;
    int index = 0;
    for (const auto& device : devices) {
        //测试Queue Families
        QueueFamilyIndices indices = findQueueFamilies(device);

        //测试Extension
        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            //测试SwapChain
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        if (indices.isComplete() && extensionsSupported && swapChainAdequate) {
            physicalDevice = device;
            whichDeviceSelected = index;
            msaaSamples = getMaxUsableSampleCount_msaa();//MSAA 1 VK_SAMPLE_COUNT_1_BIT
            break;
        }
        index++;
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }*/


/*TODO
    VkPhysicalDeviceProperties	PhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, OUT &PhysicalDeviceProperties);
    fprintf(debugger->FpDebug, "\nDevice #%d selected ('%s')\n", whichDeviceSelected, PhysicalDeviceProperties.deviceName);

    //显示选中的PhysicalDevice Features
    VkPhysicalDeviceFeatures	PhysicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(IN physicalDevice, OUT &PhysicalDeviceFeatures);

    fprintf(debugger->FpDebug, "\nPhysical Device Features:\n");
    fprintf(debugger->FpDebug, "geometryShader = %2d\n", PhysicalDeviceFeatures.geometryShader);
    fprintf(debugger->FpDebug, "tessellationShader = %2d\n", PhysicalDeviceFeatures.tessellationShader);
    fprintf(debugger->FpDebug, "multiDrawIndirect = %2d\n", PhysicalDeviceFeatures.multiDrawIndirect);
    fprintf(debugger->FpDebug, "wideLines = %2d\n", PhysicalDeviceFeatures.wideLines);
    fprintf(debugger->FpDebug, "largePoints = %2d\n", PhysicalDeviceFeatures.largePoints);
    fprintf(debugger->FpDebug, "multiViewport = %2d\n", PhysicalDeviceFeatures.multiViewport);
    fprintf(debugger->FpDebug, "occlusionQueryPrecise = %2d\n", PhysicalDeviceFeatures.occlusionQueryPrecise);
    fprintf(debugger->FpDebug, "pipelineStatisticsQuery = %2d\n", PhysicalDeviceFeatures.pipelineStatisticsQuery);
    fprintf(debugger->FpDebug, "shaderFloat64 = %2d\n", PhysicalDeviceFeatures.shaderFloat64);
    fprintf(debugger->FpDebug, "shaderInt64 = %2d\n", PhysicalDeviceFeatures.shaderInt64);
    fprintf(debugger->FpDebug, "shaderInt16 = %2d\n", PhysicalDeviceFeatures.shaderInt16);

    VkFormatProperties					vfp;
    fprintf(debugger->FpDebug, "\nImage Formats Checked:\n");
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_R32G32B32A32_SFLOAT, &vfp);
    fprintf(debugger->FpDebug, "Format VK_FORMAT_R32G32B32A32_SFLOAT: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_R8G8B8A8_UNORM, &vfp);
    fprintf(debugger->FpDebug, "Format VK_FORMAT_R8G8B8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_B8G8R8A8_UNORM, &vfp);
    fprintf(debugger->FpDebug, "Format VK_FORMAT_B8G8R8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_B8G8R8A8_SRGB, &vfp);
    fprintf(debugger->FpDebug, "Format VK_FORMAT_B8G8R8A8_SRGB: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);

    VkPhysicalDeviceMemoryProperties			vpdmp;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, OUT &vpdmp);
    fprintf(debugger->FpDebug, "\n%d Memory Types:\n", vpdmp.memoryTypeCount);
    for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
        VkMemoryType vmt = vpdmp.memoryTypes[i];
        VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
        fprintf(debugger->FpDebug, "Memory %2d: ", i);
        if ((vmpf & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)	fprintf(debugger->FpDebug, " DeviceLocal");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)	fprintf(debugger->FpDebug, " HostVisible");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0)	fprintf(debugger->FpDebug, " HostCoherent");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0)	fprintf(debugger->FpDebug, " HostCached");
        if ((vmpf & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0)	fprintf(debugger->FpDebug, " LazilyAllocated");
        fprintf(debugger->FpDebug, "\n");
    }

    fprintf(debugger->FpDebug, "\n%d Memory Heaps:\n", vpdmp.memoryHeapCount);
    for (unsigned int i = 0; i < vpdmp.memoryHeapCount; i++) {
        fprintf(debugger->FpDebug, "Heap %d: ", i);
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
    result = vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, deviceLayers);
    REPORT("vkEnumerateDeviceLayerProperties");
    fprintf(debugger->FpDebug, "\n%d physical device layers enumerated:\n", layerCount);
    for (unsigned int i = 0; i < layerCount; i++) {
        fprintf(debugger->FpDebug, "0x%08x  %2d  '%s'  '%s'\n",
            deviceLayers[i].specVersion,
            deviceLayers[i].implementationVersion,
            deviceLayers[i].layerName,
            deviceLayers[i].description);

        // see what device extensions are available:
        //这里列举的是跟某个layer相关的extension
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, deviceLayers[i].layerName, &extensionCount, (VkExtensionProperties *)nullptr);
        VkExtensionProperties * deviceExtensions = new VkExtensionProperties[extensionCount];
        result = vkEnumerateDeviceExtensionProperties(physicalDevice, deviceLayers[i].layerName, &extensionCount, deviceExtensions);
        REPORT("vkEnumerateDeviceExtensionProperties");

        fprintf(debugger->FpDebug, "\t%d device extensions enumerated for '%s':\n", extensionCount, deviceLayers[i].layerName);
        for (unsigned int ii = 0; ii < extensionCount; ii++) {
            fprintf(debugger->FpDebug, "\t0x%08x  '%s'\n",
                deviceExtensions[ii].specVersion,
                deviceExtensions[ii].extensionName);
        }
        fprintf(debugger->FpDebug, "\n");
    }
    delete[] deviceLayers;*/
}

 void CPhysicalDevice::setInstance(CInstance *instance){
    //m_instance = instance;
 }

CPhysicalDevice::~CPhysicalDevice(){
    if (!debugger) delete debugger;
}

QueueFamilyIndices CPhysicalDevice::findQueueFamilies(VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, nullptr);
    debugger->writeMSG("Found %d Queue Families:\n", queueFamilyCount);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, queueFamilyProperties.data());

    int i = 0;
    for (const auto& queueFamilyProperty : queueFamilyProperties) {
        fprintf(debugger->FpDebug, "\t%d: Queue Family Count = %2d  ;   ", i, queueFamilyProperty.queueCount);
        if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)	fprintf(debugger->FpDebug, " Graphics");
        if ((queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)	fprintf(debugger->FpDebug, " Compute");
        if ((queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)	fprintf(debugger->FpDebug, " Transfer");
        fprintf(debugger->FpDebug, "\n");
        i++;
    }

    i = 0;
    for (const auto& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
            indices.graphicsAndComputeFamily = i;
        }

        VkBool32 presentSupport = false;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surface, OUT &presentSupport);
        REPORT("vkGetPhysicalDeviceSurfaceSupportKHR");

        if (presentSupport) {
            indices.presentFamily = i;
            fprintf(debugger->FpDebug, "This Surface is supported by the Graphics Queue\n");
        }
        else fprintf(debugger->FpDebug, "This Surface is not supported by the Graphics Queue\n");

        if (indices.isComplete()) break; //select the first suitable indices (which has graphics and present family)

        i++;
    }

    return indices;
}

bool CPhysicalDevice::checkDeviceExtensionSupport(const std::vector<const char*>  requireDeviceExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(requireDeviceExtensions.begin(), requireDeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails CPhysicalDevice::querySwapChainSupport(VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, OUT &details.capabilities);
    fprintf(debugger->FpDebug, "\nvkGetPhysicalDeviceSurfaceCapabilitiesKHR:\n");
    fprintf(debugger->FpDebug, "\tminImageCount = %d ; maxImageCount = %d\n", details.capabilities.minImageCount, details.capabilities.maxImageCount);
    fprintf(debugger->FpDebug, "\tcurrentExtent = %d x %d\n", details.capabilities.currentExtent.width, details.capabilities.currentExtent.height);
    fprintf(debugger->FpDebug, "\tminImageExtent = %d x %d\n", details.capabilities.minImageExtent.width, details.capabilities.minImageExtent.height);
    fprintf(debugger->FpDebug, "\tmaxImageExtent = %d x %d\n", details.capabilities.maxImageExtent.width, details.capabilities.maxImageExtent.height);
    fprintf(debugger->FpDebug, "\tmaxImageArrayLayers = %d\n", details.capabilities.maxImageArrayLayers);
    fprintf(debugger->FpDebug, "\tsupportedTransforms = 0x%04x\n", details.capabilities.supportedTransforms);
    fprintf(debugger->FpDebug, "\tcurrentTransform = 0x%04x\n", details.capabilities.currentTransform);
    fprintf(debugger->FpDebug, "\tsupportedCompositeAlpha = 0x%04x\n", details.capabilities.supportedCompositeAlpha);
    fprintf(debugger->FpDebug, "\tsupportedUsageFlags = 0x%04x\n", details.capabilities.supportedUsageFlags);


    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, nullptr);
    debugger->writeMSG("\nFound %d Surface Formats:\n", formatCount);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, details.formats.data());
    }
    for (uint32_t i = 0; i < formatCount; i++) {
        fprintf(debugger->FpDebug, "%3d:     %4d     %12d", i, details.formats[i].format, details.formats[i].colorSpace); \
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_SRGB_NONLINEAR_KHR\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_DCI_P3_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DCI_P3_LINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_BT709_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_BT709_LINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_BT709_NONLINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_BT709_NONLINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_BT2020_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_BT2020_LINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_HDR10_ST2084_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_DOLBYVISION_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DOLBYVISION_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_HDR10_HLG_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_HDR10_HLG_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_ADOBERGB_LINEAR_EXT\n");
        if (details.formats[i].colorSpace == VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT\n");

    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, nullptr);
    debugger->writeMSG("\nFound %d Present Modes:\n", presentModeCount);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, details.presentModes.data());
    }
    for (uint32_t i = 0; i < presentModeCount; i++) {
        fprintf(debugger->FpDebug, "%3d:     %4d", i, details.presentModes[i]);
        if (details.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_IMMEDIATE_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_MAILBOX_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_FIFO_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)		fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_FIFO_RELAXED_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR)	fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR)	fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR\n");
    }
    fprintf(stderr, "\n");

    return details;
}

void CPhysicalDevice::createLogicalDevices(){
    //physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
    logicalDevices.push_back(std::make_unique<CLogicalDevice>());

    /*
    HERE_I_AM("Init04LogicalDeviceAndQueue");

    VkResult result = VK_SUCCESS;

    //QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    QueueFamilyIndices indices = instance->findQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    //建立logicalDevice
    result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create logical device!");
    REPORT("vkCreateLogicalDevice");

    //把PhysicalDevice的Queue Familily属性指定到logicalDevice里对应的queue里
    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue); //graphics queue use physical device's family 0 
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue); //present queue use the same family
                                                                                        //A physical device has several family, queue is pointing to one of the families
    vkGetDeviceQueue(logicalDevice, indices.graphicsAndComputeFamily.value(), 0, &computeQueue);
    */
}


/*Below are MSAA related functions TODO
    VkSampleCountFlagBits getMaxUsableSampleCount_msaa() {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }*/

    
