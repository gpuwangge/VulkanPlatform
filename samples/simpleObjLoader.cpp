#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleObjLoader
class TEST_CLASS_NAME: public CApplication{
public:
	CObject object;
    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		
		swapchain.bEnableDepthTest = true; 
		
		modelManager.LoadObjModel("viking_room.obj", object.vertices3D, object.indices3D);

		renderer.CreateVertexBuffer<Vertex3D>(object.vertices3D);
		renderer.CreateIndexBuffer(object.indices3D);
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;	
		textureManager.CreateTextureImage("viking_room.png", usage, renderer.commandPool);

		VkFormat depthFormat = renderProcess.findDepthFormat();
		usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		swapchain.createDepthImages(depthFormat, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		swapchain.createDepthImageViews(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.addDepthAttachment(); //add this function will enable depth attachment(bUseDepthAttachment = true)
		renderProcess.createSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleObjLoader/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleObjLoader/frag.spv", shaderManager.FRAG);

		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);

		CDescriptorManager::createDescriptorPool();

		CGraphicsDescriptorManager::createDescriptorSetLayout();
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout();

		graphicsDescriptorManager.createDescriptorSets();
		object.createTextureDescriptorSets(
			textureManager.textureImages[object.id], 
			CGraphicsDescriptorManager::descriptorPool,
			CGraphicsDescriptorManager::textureDescriptorSetLayout,
			CGraphicsDescriptorManager::textureSamplers[0]
		);

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
		dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

		renderProcess.createGraphicsPipelineLayout(dsLayouts);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		drawObject(0);
	}

	void drawObject(int objectId){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		dsSets.push_back(graphicsDescriptorManager.descriptorSets);
		dsSets.push_back(object.descriptorSets); //set = 1

		renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, dsSets, objectId);
		renderer.BindVertexBuffer(objectId);
		renderer.BindIndexBuffer(objectId);
		renderer.DrawIndexed(object.indices3D);
	}	
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif