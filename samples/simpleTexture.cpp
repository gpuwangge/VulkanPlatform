#include "..\\framework\\include\\vulkanBase.h"
#define TEST_CLASS_NAME CSimpleTexture
class TEST_CLASS_NAME: public CVulkanBase{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
	};

	/************
	buffer size: 6*4=24（byte）
	************/
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

    void initialize(){
		//Create buffers
		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);
		renderer.InitCreateCommandPool(surface);
		renderer.InitCreateCommandBuffers();

		//Create texture resource
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		wxjCreateImage_texture("../textures/texture.jpg", usage, textureImageBuffer, texWidth, texHeight);
		wxjCreateImageView(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, OUT textureImageView);

		wxjCreateSwapChainImagesAndImageViews();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		wxjCreateFramebuffers();

		//Create shader resource
		shaderManager.InitVertexShader("../shaders/texture/vert.spv");
		shaderManager.InitFragmentShader("../shaders/texture/frag.spv");

		//Create Descriptors
		descriptor.addMVPUniformBuffer();
		descriptor.addImageSamplerUniformBuffer(mipLevels);
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(&textureImageView);

		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule, 
			descriptor.descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordCommandBuffer(){
		renderer.BeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  1.0f, 1.0f, 1.0f, 1.0f  } };
		renderer.BeginRenderPass(renderProcess.renderPass, swapchain.swapChainFramebuffers, swapchain.swapChainExtent, clearValues);

		renderer.BindPipeline(renderProcess.graphicsPipeline);
		renderer.SetViewport(swapchain.swapChainExtent);
		renderer.SetScissor(swapchain.swapChainExtent);
		renderer.BindVertexBuffer();
		renderer.BindIndexBuffer();
		renderer.BindDescriptorSets(renderProcess.pipelineLayout, descriptor.descriptorSets);
		renderer.DrawIndexed(indices3D);

		renderer.EndRenderPass();
		renderer.EndCOmmandBuffer();

		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"