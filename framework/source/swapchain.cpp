#include "swapchain.h"

CSwapchain::CSwapchain(){
    debugger = new CDebugger("../logs/swapchain.log");
}
CSwapchain::~CSwapchain(){
    if (!debugger) delete debugger;
}

// void CSwapchain::GetPhysicalDevice(CPhysicalDevice *physical_device){
//     m_physical_device = physical_device;
// }

void CSwapchain::createSwapchainImages(VkSurfaceKHR surface, int width, int height){
    //vulkan draws on the vkImage(s)
    //SwapChain will set vkImage to present on the screen
    //Surface will tell the format of the vkImage
    //PresentQueue is a queue to present

    //Need preare surface and presentQueue first.
    //swapchain.GetPhysicalDevice(CContext::GetHandle().physicalDevice->get());

    VkResult result = VK_SUCCESS;

    //SwapChainSupportDetails swapChainSupport = instance->pickedPhysicalDevice->get()->querySwapChainSupport(surface);
    SwapChainSupportDetails swapChainSupport = CContext::GetHandle().physicalDevice->get()->querySwapChainSupport(surface);
    displaySwapchainInfo(swapChainSupport);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    //QueueFamilyIndices indices = instance->pickedPhysicalDevice->get()->findQueueFamilies(surface);
    QueueFamilyIndices indices = CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    //generate swapChain images (swapChainImages)
    //result = vkCreateSwapchainKHR(LOGICAL_DEVICE, &createInfo, nullptr, &swapChain);
    result = vkCreateSwapchainKHR(CContext::GetHandle().GetLogicalDevice(), &createInfo, nullptr, &handle);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create swap chain!");
    REPORT("vkCreateSwapchainKHR");

    result = vkGetSwapchainImagesKHR(CContext::GetHandle().GetLogicalDevice(), handle, &imageCount, nullptr);
    REPORT("vkGetSwapchainImagesKHR(Get imageCount)");
    swapChainImages.resize(imageCount);
    result = vkGetSwapchainImagesKHR(CContext::GetHandle().GetLogicalDevice(), handle, &imageCount, swapChainImages.data());
    REPORT("vkGetSwapchainImagesKHR");

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    // present views for the double-buffering:
    swapChainImageViews.resize(swapChainImages.size());
}

void CSwapchain::displaySwapchainInfo(SwapChainSupportDetails details){
    HERE_I_AM("querySwapChainSupport");

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
   
    debugger->writeMSG("\nFound %d Surface Formats:\n", details.formats.size());
    for (uint32_t i = 0; i < details.formats.size(); i++) {
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

    debugger->writeMSG("\nFound %d Present Modes:\n", details.presentModes.size());
    for (uint32_t i = 0; i < details.presentModes.size(); i++) {
        fprintf(debugger->FpDebug, "%3d:     %4d", i, details.presentModes[i]);
        if (details.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_IMMEDIATE_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_MAILBOX_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_FIFO_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)		fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_FIFO_RELAXED_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR)	fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR\n");
        if (details.presentModes[i] == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR)	fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR\n");
    }

    debugger->writeMSG("\n");

    //fprintf(stderr, "\n");
}

VkSurfaceFormatKHR CSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR CSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D CSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        //int width, height;
        //glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}


void CSwapchain::CleanUp(){
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(CContext::GetHandle().GetLogicalDevice(), framebuffer, nullptr);
    }

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(CContext::GetHandle().GetLogicalDevice(), handle, nullptr);
}