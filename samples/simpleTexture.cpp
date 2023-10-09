#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleTexture
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	std::vector<VkClearValue> clearValues{ {  1.0f, 1.0f, 1.0f, 1.0f  } };

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureImages[0].CreateTextureImage("texture.jpg", usage, renderer.commandPool);
		textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.CreateVertexShader("simpleTexture/vert.spv");
		//shaderManager.CreateFragmentShader("simpleTexture/frag.spv");
		shaderManager.CreateShader("simpleTexture/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleTexture/frag.spv", shaderManager.FRAG); 

		descriptors[0].addMVPUniformBuffer();
		descriptors[0].addImageSamplerUniformBuffer(textureImages[0].mipLevels);
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets(&textureImages);
		
		renderProcess.createGraphicsPipelineLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);
		
		CApplication::initialize();
	}

	void update(){
		descriptors[0].mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)

		renderer.BindVertexBuffer();
		renderer.BindIndexBuffer();
		renderer.DrawIndexed(indices3D);
		
		END_GRAPHICS_RECORD
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif