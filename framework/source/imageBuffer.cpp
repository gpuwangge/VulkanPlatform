#include "imageBuffer.h"

CWxjImageBuffer::CWxjImageBuffer(){
    debugger = new CDebugger("../logs/imageBuffer.log");
    size = 0;
}

CWxjImageBuffer::~CWxjImageBuffer(){
    if (!debugger) delete debugger;
}

void CWxjImageBuffer::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
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

    if (vkCreateImage(CContext::GetHandle().GetLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(CContext::GetHandle().GetLogicalDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(CContext::GetHandle().GetLogicalDevice(), &allocInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    size = allocInfo.allocationSize;

    vkBindImageMemory(CContext::GetHandle().GetLogicalDevice(), image, deviceMemory, 0);
}

VkImageView CWxjImageBuffer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels){
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

void CWxjImageBuffer::createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
    view = createImageView(image, format, aspectFlags, mipLevels);
}

uint32_t CWxjImageBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(CContext::GetHandle().GetPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}


void CWxjImageBuffer::destroy(){
    if(size != (VkDeviceSize)0){
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), deviceMemory, nullptr);
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), view, nullptr);
    }
}