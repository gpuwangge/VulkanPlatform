#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


CTextureImage::CTextureImage(){}
CTextureImage::~CTextureImage(){}

void CTextureImage::CreateImage_texture(const std::string texturePath, VkImageUsageFlags usage, 
        OUT MyImageBuffer &textureImageBuffer, OUT int32_t &texWidth, OUT int32_t &texHeight, VkCommandPool &commandPool) {
	HERE_I_AM("Init07CreateTextureImage");

	int texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	mipLevels = bEnableMipMap ? (static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1) : 1;

	if (!pixels) throw std::runtime_error("failed to load texture image!");


	CWxjBuffer stagingBuffer;
	//VkResult result = InitDataBufferHelper(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &stagingBuffer);
	VkResult result = stagingBuffer.init(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	REPORT("InitTextureImageBuffer");
	//FillDataBufferHelper(stagingBuffer, (void *)(vertices.data()));

	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//FillDataBufferHelper(stagingBuffer, pixels);
	stagingBuffer.fill(pixels);
	//void* data;
	//vkMapMemory(logicalDevice, stagingBuffer.deviceMemory, 0, imageSize, 0, &data);
	//memcpy(data, pixels, static_cast<size_t>(imageSize));
	//vkUnmapMemory(logicalDevice, stagingBuffer.deviceMemory);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImageBuffer);

	//hallway mipmap use this:
	//transitionImageLayout(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
	//other use this:
	//TODO:
	if(mipLevels == 1){
		transitionImageLayout(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, commandPool);
 		copyBufferToImage(stagingBuffer.buffer, textureImageBuffer.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool);
      	transitionImageLayout(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, commandPool);
	}else{
		transitionImageLayout(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, commandPool);
		copyBufferToImage(stagingBuffer.buffer, textureImageBuffer.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool);

	}
	//MIPMAP TODO: no need transition?? If comment out this, validation layer get error
	//transitionImageLayout(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	vkDestroyBuffer(CContext::GetHandle().GetLogicalDevice(), stagingBuffer.buffer, nullptr);
	vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), stagingBuffer.deviceMemory, nullptr);
}

void CTextureImage::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkCommandPool &commandPool) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer, commandPool);
}

void CTextureImage::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool &commandPool) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer, commandPool);
}

VkCommandBuffer CTextureImage::beginSingleTimeCommands(VkCommandPool &commandPool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(CContext::GetHandle().GetLogicalDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CTextureImage::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool &commandPool) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(CContext::GetHandle().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(CContext::GetHandle().GetGraphicsQueue());

    vkFreeCommandBuffers(CContext::GetHandle().GetLogicalDevice(), commandPool, 1, &commandBuffer);
}

void CTextureImage::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, std::array<MyImageBuffer, MIPMAP_TEXTURE_COUNT> *textureImageBuffers_mipmaps, bool bMix, VkCommandPool &commandPool) {
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(CContext::GetHandle().GetPhysicalDevice(), imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		if (bMix) {
            //TODO: (Validation Error)
            //command buffer VkCommandBuffer 0x62f45f8[] expects VkImage 0xab64de0000000020[] 
            //(subresource: aspectMask 0x1 array layer 0, mip level 3) to be in layout VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL--instead, 
            //current layout is VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			int j = i > MIPMAP_TEXTURE_COUNT ? MIPMAP_TEXTURE_COUNT : i;
			vkCmdBlitImage(commandBuffer,
				(*textureImageBuffers_mipmaps)[j-1].image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);
		}else {
			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);
		}

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        //barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	endSingleTimeCommands(commandBuffer, commandPool);
}
void CTextureImage::Destroy(){
    if(textureImageBuffer.size != (VkDeviceSize)0){
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), textureImageBuffer.image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), textureImageBuffer.deviceMemory, nullptr);
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), textureImageView, nullptr);
    }
}

void CTextureImage::CreateRainbowMipmaps(IN OUT VkImage image, VkImageUsageFlags usage, std::string rainbowCheckerboardTexturePath, VkCommandPool &commandPool){ //rainbow mipmaps case
	std::array<MyImageBuffer, MIPMAP_TEXTURE_COUNT> tmpTextureBufferForRainbowMipmaps;//create temp mipmaps
	for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {//fill temp mipmaps
		//wxjCreateImage_texture(rainbowCheckerboardTexturePath + std::to_string(i) + ".png", usage, tmpTextureBufferForRainbowMipmaps[i], texWidth, texHeight);
        CreateImage_texture(rainbowCheckerboardTexturePath + std::to_string(i) + ".png", usage, tmpTextureBufferForRainbowMipmaps[i], texWidth, texHeight, commandPool);
		generateMipmaps(tmpTextureBufferForRainbowMipmaps[i].image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, &tmpTextureBufferForRainbowMipmaps, false, commandPool);
	}
	//use tmp mipmaps texture to update main texture
	generateMipmaps(image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, &tmpTextureBufferForRainbowMipmaps, true, commandPool);
	//Clean up
	for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {
		vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), tmpTextureBufferForRainbowMipmaps[i].image, nullptr);
		vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), tmpTextureBufferForRainbowMipmaps[i].deviceMemory, nullptr);
	}
}


