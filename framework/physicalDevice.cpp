#include "physicalDevice.h"

#include "instance.h"

CPhysicalDevice::CPhysicalDevice(VkPhysicalDevice physical_device) : handle(physical_device) {
//CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device){
    debugger = new CDebugger("../logs/physicalDeviceLog.txt");
}

 void CPhysicalDevice::setInstance(CInstance *instance){
    //m_instance = instance;
 }

CPhysicalDevice::~CPhysicalDevice(){
    if (!debugger) delete debugger;
}

QueueFamilyIndices CPhysicalDevice::findQueueFamilies(VkSurfaceKHR surface) {
    HERE_I_AM("findQueueFamilies");

    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, nullptr);
    debugger->writeMSG("Found %d Queue Families:\n", queueFamilyCount);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, queueFamilyProperties.data());

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
    }

    i = 0;
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
        REPORT("vkGetPhysicalDeviceSurfaceSupportKHR");

        if (presentSupport) {
            indices.presentFamily = i;
            debugger->writeMSG("This Surface is supported by the Present Queue\n");
        }
        else debugger->writeMSG("This Surface is not supported by the Present Queue\n");

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
    HERE_I_AM("querySwapChainSupport");

    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, OUT &details.capabilities);
    fprintf(debugger->FpDebug, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR:\n");
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

void CPhysicalDevice::createLogicalDevices(VkSurfaceKHR surface, const std::vector<const char*> requiredValidationLayers, const std::vector<const char*>  requireDeviceExtensions){
    HERE_I_AM("createLogicalDevices");
    
    //physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
    logicalDevices.push_back(std::make_unique<CLogicalDevice>());//create one logicalDevice for each invocation of this function

    VkResult result = VK_SUCCESS;

    //QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    QueueFamilyIndices indices = findQueueFamilies(surface);

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

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requireDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = requireDeviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    //建立logicalDevice
    result = vkCreateDevice(handle, &createInfo, nullptr, &(logicalDevices.back().get()->logicalDevice));
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create logical device!");
    REPORT("vkCreateLogicalDevice");

    //把PhysicalDevice的Queue Familily属性指定到logicalDevice里对应的queue里
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.graphicsFamily.value(), 0, &(logicalDevices.back().get()->graphicsQueue)); //graphics queue use physical device's family 0 
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.presentFamily.value(), 0, &(logicalDevices.back().get()->presentQueue)); //present queue use the same family
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.graphicsAndComputeFamily.value(), 0, &(logicalDevices.back().get()->computeQueue));//A physical device has several family, queue is pointing to one of the families
    
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

    
