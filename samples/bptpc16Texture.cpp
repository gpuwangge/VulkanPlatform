#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CBpt16Texture
/************************************************************
*A sample to test texture with 16 bit per texel(bpt) per channel(pc)
************************************************************/
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	std::vector<VkClearValue> clearValues{ {  1.0f, 1.0f, 1.0f, 1.0f  } };

    void initialize(){
		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);
		renderer.CreateCommandPool(surface);
		renderer.CreateCommandBuffers();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureImages[0].CreateTextureImage("48bpt.png", usage, renderer.commandPool, 16);
		textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateVertexShader("simpleTexture/vert.spv");
		shaderManager.CreateFragmentShader("simpleTexture/frag.spv");

		descriptor.addMVPUniformBuffer();
		descriptor.addImageSamplerUniformBuffer(textureImages[0].mipLevels);
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(textureImages);
		
		renderProcess.createLayout(descriptor.descriptorSetLayout);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);
		
		CApplication::initialize();
	}

	void update(){
		//descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordCommandBuffer(){
		RENDER_START

		renderer.BindVertexBuffer();
		renderer.BindIndexBuffer();
		renderer.DrawIndexed(indices3D);
		
		RENDER_END
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif