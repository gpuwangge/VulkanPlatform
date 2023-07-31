#include "swapchain.h"

CSwapchain::CSwapchain(){}
CSwapchain::~CSwapchain(){}

void CSwapchain::GetPhysicalDevice(CPhysicalDevice *physical_device){
    m_physical_device = physical_device;
}

void CSwapchain::queryInfo(VkSurfaceKHR surface, int width, int height){
    VkResult result = VK_SUCCESS;

    //SwapChainSupportDetails swapChainSupport = instance->pickedPhysicalDevice->get()->querySwapChainSupport(surface);
    SwapChainSupportDetails swapChainSupport = m_physical_device->querySwapChainSupport(surface);

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
    QueueFamilyIndices indices = m_physical_device->findQueueFamilies(surface);
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
    result = vkCreateSwapchainKHR(*(m_physical_device->getLogicalDevice()), &createInfo, nullptr, &handle);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create swap chain!");
    //REPORT("vkCreateSwapchainKHR");

    result = vkGetSwapchainImagesKHR(*(m_physical_device->getLogicalDevice()), handle, &imageCount, nullptr);
    //REPORT("vkGetSwapchainImagesKHR(Get imageCount)");
    swapChainImages.resize(imageCount);
    result = vkGetSwapchainImagesKHR(*(m_physical_device->getLogicalDevice()), handle, &imageCount, swapChainImages.data());
    //REPORT("vkGetSwapchainImagesKHR");

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    // present views for the double-buffering:
    swapChainImageViews.resize(swapChainImages.size());
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
