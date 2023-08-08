#include "imageManager.h"

CImageManager::CImageManager(){
    debugger = new CDebugger("../logs/imageManager.log");
}

CImageManager::~CImageManager(){
    if (!debugger) delete debugger;
}

void CImageManager::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, MyImageBuffer &imageBuffer) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(CContext::GetHandle().GetLogicalDevice(), &imageInfo, nullptr, &imageBuffer.image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(CContext::GetHandle().GetLogicalDevice(), imageBuffer.image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(CContext::GetHandle().GetLogicalDevice(), &allocInfo, nullptr, &imageBuffer.deviceMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    imageBuffer.size = allocInfo.allocationSize;

    vkBindImageMemory(CContext::GetHandle().GetLogicalDevice(), imageBuffer.image, imageBuffer.deviceMemory, 0);
}

VkImageView CImageManager::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
    //Concept of imageView that something can change how we present the image, but no need to change the image itself
    //Imageviews are commonly used in Vulkan, all images should have their imageviews.

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;

    //Components change texture color for any channel
    //VK_COMPONENT_SWIZZLE_IDENTITY is default value
    //VK_COMPONENT_SWIZZLE_ZERO will set channel value to 0
    //VK_COMPONENT_SWIZZLE_R will set the channel value to r channel's value
    //VK_COMPONENT_SWIZZLE_ONE will set the channel value to maximum
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(CContext::GetHandle().GetLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

uint32_t CImageManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(CContext::GetHandle().GetPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}







CSwapchainImageManager::CSwapchainImageManager(){
    imageSize = 0;
    bEnableMSAA = false;
    msaaSamples = VK_SAMPLE_COUNT_1_BIT;
}
CSwapchainImageManager::~CSwapchainImageManager(){}

void CSwapchainImageManager::GetMaxUsableSampleCount(){
	msaaSamples = CContext::GetHandle().physicalDevice->get()->getMaxUsableSampleCount();
	//msaaSamples = VK_SAMPLE_COUNT_1_BIT;
}



void CSwapchainImageManager::Destroy(){//TODO
    if(bEnableDepthTest){
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), depthImageBuffer.image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), depthImageBuffer.deviceMemory, nullptr);
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), depthImageView, nullptr);
    }
    if(bEnableMSAA){
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), msaaColorImageView, nullptr);
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), msaaColorImageBuffer.image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), msaaColorImageBuffer.deviceMemory, nullptr);
    }
}
