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

	CObject triangleObject;

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		triangleObject.InitVertices3D(vertices3D);
		triangleObject.InitIndices3D(indices3D);

		renderer.CreateVertexBuffer<Vertex3D>(triangleObject.vertices3D);
		renderer.CreateIndexBuffer(triangleObject.indices3D);

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("texture.jpg", usage, renderer.commandPool);
		//textureManager.textureImages[0].CreateTextureImage("texture.jpg", usage, renderer.commandPool);
		//textureManager.textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleTexture/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleTexture/frag.spv", shaderManager.FRAG); 

		CGraphicsDescriptorManager::addMVPUniformBuffer();
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(textureManager.textureImages[0].mipLevels);
		CDescriptorManager::createDescriptorPool(); //pool size = 2
		//CDescriptor::createMVPDescriptorSetLayout();
		CGraphicsDescriptorManager::createDescriptorSetLayout(); //layout size = 1
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); //layout size = 1
		//descriptors[0].createMVPDescriptorSets();
		//descriptors[0].createDescriptorSets(); //set size = 1
		graphicsDescriptorManager.createDescriptorSets();
		triangleObject.createTextureDescriptorSets(
			textureManager.textureImages[triangleObject.id], 
			CGraphicsDescriptorManager::descriptorPool,
			CGraphicsDescriptorManager::textureDescriptorSetLayout,
			CGraphicsDescriptorManager::textureSamplers[0]);
		//textureDescriptor.createTextureDescriptorSets(textureManager.textureImages[0], textureDescriptor.descriptorSets); //set size = 1
		
		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		//dsLayouts.push_back(CDescriptor::mvpDescriptorSetLayout); //set = 0
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout); //set = 0
		dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

		renderProcess.createGraphicsPipelineLayout(dsLayouts);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);
		
		CApplication::initialize();
	}

	void update(){
		//descriptors[0].mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)

		drawObject(0);
		
		END_GRAPHICS_RECORD
	}

	void drawObject(int objectId){
		//2d vector: dsSets[different sets][host resources]
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		dsSets.push_back(graphicsDescriptorManager.descriptorSets); //set = 0
		dsSets.push_back(triangleObject.descriptorSets); //set = 1

		//support multiple descriptors in one piplines: bind multiple descriptor sets in one pipeline
		renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, dsSets, objectId);//descriptors[0].descriptorSets
		renderer.BindVertexBuffer(objectId);
		renderer.BindIndexBuffer(objectId);
		renderer.DrawIndexed(indices3D);
	}	
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif