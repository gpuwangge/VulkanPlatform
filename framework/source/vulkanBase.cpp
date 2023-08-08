#include "vulkanBase.h"



#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void CVulkanBase::wxjCreateFramebuffers(){
	HERE_I_AM("wxjCreateFramebuffers");

	VkResult result = VK_SUCCESS;

	swapchain.swapChainFramebuffers.resize(swapchain.swapchainImage.swapChainImageViews.size());

	for (size_t i = 0; i < swapchain.swapchainImage.swapChainImageViews.size(); i++) {
		std::vector<VkImageView> attachments; 
		 if (renderProcess.bUseDepthAttachment && renderProcess.bUseColorAttachmentResolve) {//Renderpass attachment(render target) order: Color, Depth, ColorResolve
		    attachments.push_back(swapchain.swapchainImage.msaaColorImageView);
		    attachments.push_back(swapchain.swapchainImage.depthImageView);
			attachments.push_back(swapchain.swapchainImage.swapChainImageViews[i]);
		 }else if(renderProcess.bUseDepthAttachment){//Renderpass attachment(render target) order: Color, Depth
			attachments.push_back(swapchain.swapchainImage.swapChainImageViews[i]);
			attachments.push_back(swapchain.swapchainImage.depthImageView);
		}else{ //Renderpass attachment(render target) order: Color
			attachments.push_back(swapchain.swapchainImage.swapChainImageViews[i]);
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderProcess.renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();//swapChainImageViews, 类型是VkImageView
		framebufferInfo.width = swapchain.swapChainExtent.width;
		framebufferInfo.height = swapchain.swapChainExtent.height;
		framebufferInfo.layers = 1;

		result = vkCreateFramebuffer(CContext::GetHandle().GetLogicalDevice(), &framebufferInfo, nullptr, &swapchain.swapChainFramebuffers[i]);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create framebuffer!");
		REPORT("vkCreateFrameBuffer");
	}	
}



// void CVulkanBase::wxjCreateImageView(IN VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, int mipLevel, OUT VkImageView &imageView){
//     imageView = createImageView(image, format, aspectFlags, mipLevel);
// }
// void CVulkanBase::wxjCreateImage(VkSampleCountFlagBits numSamples, VkFormat format, VkImageUsageFlags usage, OUT MyImageBuffer &imageBuffer){
// 	//do not use mipLevels here, because only depth and msaa(colorResolve) buffer calls this, they dont need mipmap
// 	//texture (w/o mipmap) has another function to create image
// 	createImage(swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, 1, numSamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageBuffer);
// }

void CVulkanBase::wxjCreateSwapChainImagesAndImageViews(){
	HERE_I_AM("Init08Swapchain");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    swapchain.createSwapchainImages(surface, width, height);

    //generate swapChainImageViews
    for (size_t i = 0; i < swapchain.swapchainImage.swapChainImages.size(); i++) {
        //swapchain.swapchainImage.swapChainImageViews[i] = createImageView(swapchain.swapChainImages[i], swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		swapchain.swapchainImage.swapChainImageViews[i] = swapchain.swapchainImage.createImageView(swapchain.swapchainImage.swapChainImages[i], swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        //REPORT("vkCreateImageView");
    }
}

void CVulkanBase::wxjLoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelName.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex3D, uint32_t> uniqueVertices{};

	//vertices3D.clear();
	//indices3D.clear();

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex3D vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices3D.size());
				vertices3D.push_back(vertex);
			}

			indices3D.push_back(uniqueVertices[vertex]);
		}
	}
}


// void CVulkanBase::wxjCreateMipmaps(IN OUT VkImage image){ //normal mipmap case
// 	generateMipmaps(image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, NULL, false);
// }

// void CVulkanBase::wxjCreateMipmaps(IN OUT VkImage image, VkImageUsageFlags usage, std::string rainbowCheckerboardTexturePath){ //rainbow mipmaps case
// 	std::array<MyImageBuffer, MIPMAP_TEXTURE_COUNT> tmpTextureBufferForRainbowMipmaps;//create temp mipmaps
// 	for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {//fill temp mipmaps
// 		wxjCreateImage_texture(rainbowCheckerboardTexturePath + std::to_string(i) + ".png", usage, tmpTextureBufferForRainbowMipmaps[i], texWidth, texHeight);
// 		generateMipmaps(tmpTextureBufferForRainbowMipmaps[i].image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, &tmpTextureBufferForRainbowMipmaps, false);
// 	}
// 	//use tmp mipmaps texture to update main texture
// 	generateMipmaps(image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, &tmpTextureBufferForRainbowMipmaps, true);
// 	//Clean up
// 	for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {
// 		vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), tmpTextureBufferForRainbowMipmaps[i].image, nullptr);
// 		vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), tmpTextureBufferForRainbowMipmaps[i].deviceMemory, nullptr);
// 	}
// }







