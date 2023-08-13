#include "../include/physicalDevice.h"
//#include "../include/instance.h"

CPhysicalDevice::CPhysicalDevice(VkPhysicalDevice physical_device) : handle(physical_device) {
    //CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device){
    //debugger = new CDebugger("../logs/physicalDevice.log");
}

 //void CPhysicalDevice::setInstance(CInstance *instance){
    //m_instance = instance;
 //}

CPhysicalDevice::~CPhysicalDevice(){
    //if (!debugger) delete debugger;
}

QueueFamilyIndices CPhysicalDevice::findQueueFamilies(VkSurfaceKHR surface, std::string s) {
    //HERE_I_AM(s);

    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, nullptr);
    //debugger->writeMSG("Found %d Queue Families:\n", queueFamilyCount);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, queueFamilyProperties.data());

    /*
    int i = 0;
    if(debugger->getVerbose()){
        for (const auto& queueFamilyProperty : queueFamilyProperties) {
            fprintf(debugger->FpDebug, "\t%d: Queue Family Count = %2d  ;   ", i, queueFamilyProperty.queueCount);
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)	fprintf(debugger->FpDebug, " Graphics");
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)	fprintf(debugger->FpDebug, " Compute");
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)	fprintf(debugger->FpDebug, " Transfer");
            fprintf(debugger->FpDebug, "\n");
            i++;
        }
    }*/

    int i = 0;
    bool selected = false;
    for (const auto& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.computeFamily = i;
        }

        if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
            indices.graphicsAndComputeFamily = i;
        }

        VkBool32 presentSupport = false;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surface, OUT &presentSupport);
        //REPORT("vkGetPhysicalDeviceSurfaceSupportKHR");

        if (presentSupport) {
            indices.presentFamily = i;
            //debugger->writeMSG("This Surface is supported by the Present Queue\n");
        }
        //else debugger->writeMSG("This Surface is not supported by the Present Queue\n");

        if (indices.isComplete()) {
            selected = true;
            //debugger->writeMSG("Complete: selected queue family %d that support Graphics, Present and Compute command.\n", i);
            break; //select the first suitable indices (which has graphics and present family)
        }
        i++;
    }
    
    //if(!selected) debugger->writeMSG("Can NOT find proper queue family!\n");
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
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, details.formats.data());
    }


    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void CPhysicalDevice::createLogicalDevices(VkSurfaceKHR surface, const std::vector<const char*> requiredValidationLayers, const std::vector<const char*>  requireDeviceExtensions){
    //HERE_I_AM("createLogicalDevices");
    
    //physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
    logicalDevices.push_back(std::make_unique<CLogicalDevice>());//create one logicalDevice for each invocation of this function

    VkResult result = VK_SUCCESS;

    QueueFamilyIndices indices = findQueueFamilies(surface, "Find Queue Families when creating logical device");

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
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requireDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = requireDeviceExtensions.data();

#ifndef ANDROID
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    }
    else
#endif 
        createInfo.enabledLayerCount = 0;
    

    //create logicalDevice
    result = vkCreateDevice(handle, &createInfo, nullptr, &(logicalDevices.back().get()->logicalDevice));
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create logical device!");
    //REPORT("vkCreateLogicalDevice");

    //set PhysicalDevice's Queue Familily property to logicalDevice's queue
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.graphicsFamily.value(), 0, &(logicalDevices.back().get()->graphicsQueue)); //graphics queue use physical device's family 0 
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.presentFamily.value(), 0, &(logicalDevices.back().get()->presentQueue)); //present queue use the same family
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.graphicsAndComputeFamily.value(), 0, &(logicalDevices.back().get()->computeQueue));//A physical device has several family, queue is pointing to one of the families
    
}

//MSAA related functions 
VkSampleCountFlagBits CPhysicalDevice::getMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(handle, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

void CPhysicalDevice::displayPhysicalDevices(){
    /*
    HERE_I_AM("displayPhysicalDevices");

    VkPhysicalDeviceProperties	PhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(IN handle, OUT &PhysicalDeviceProperties);

    //fprintf(debugger->FpDebug, "\nDevice %2d:\n", index);
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
    vkGetPhysicalDeviceFeatures(IN handle, OUT &PhysicalDeviceFeatures);

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
    fprintf(debugger->FpDebug, "\tsamplerAnisotropy = %2d\n", PhysicalDeviceFeatures.samplerAnisotropy);
    

    VkFormatProperties					vfp;
    fprintf(debugger->FpDebug, "\n\tImage Formats Checked:\n");
    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_R32G32B32A32_SFLOAT, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_R32G32B32A32_SFLOAT: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_R8G8B8A8_UNORM, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_R8G8B8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_B8G8R8A8_UNORM, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_B8G8R8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_B8G8R8A8_SRGB, &vfp);
    fprintf(debugger->FpDebug, "\tFormat VK_FORMAT_B8G8R8A8_SRGB: 0x%08x 0x%08x  0x%08x\n",
        vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);

    VkPhysicalDeviceMemoryProperties			vpdmp;
    vkGetPhysicalDeviceMemoryProperties(handle, OUT &vpdmp);
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
    vkEnumerateDeviceLayerProperties(handle, &layerCount, (VkLayerProperties *)nullptr);
    VkLayerProperties * deviceLayers = new VkLayerProperties[layerCount];
    VkResult result = vkEnumerateDeviceLayerProperties(handle, &layerCount, deviceLayers);
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
        vkEnumerateDeviceExtensionProperties(handle, deviceLayers[i].layerName, &extensionCount, (VkExtensionProperties *)nullptr);
        VkExtensionProperties * deviceExtensions = new VkExtensionProperties[extensionCount];
        result = vkEnumerateDeviceExtensionProperties(handle, deviceLayers[i].layerName, &extensionCount, deviceExtensions);
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
    debugger->flush();*/
}
    
