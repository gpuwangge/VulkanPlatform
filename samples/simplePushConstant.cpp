#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimplePushConstant
#include "object.h"
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } }
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
 
	CObject object;

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, 0.0f, -1.5f));

		//triangleObject.InitVertices3D(vertices3D);
		//triangleObject.InitIndices3D(indices3D);

		//renderer.CreateVertexBuffer<Vertex3D>(triangleObject.vertices3D);
		//renderer.CreateIndexBuffer(triangleObject.indices3D);

		object.Init((CApplication*)this, vertices3D, indices3D);

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("texture.jpg", usage, renderer.commandPool);	

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simplePushConstant/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simplePushConstant/frag.spv", shaderManager.FRAG); 
		
		shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);

		CGraphicsDescriptorManager::addVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);

		CDescriptorManager::createDescriptorPool(); 

		CGraphicsDescriptorManager::createDescriptorSetLayout(); 
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); 

		graphicsDescriptorManager.createDescriptorSets();
		object.CreateTextureDescriptorSets(
			textureManager.textureImages[object.GetID()], 
			CGraphicsDescriptorManager::descriptorPool,
			CGraphicsDescriptorManager::textureDescriptorSetLayout,
			CGraphicsDescriptorManager::textureSamplers[0],
			CGraphicsDescriptorManager::CheckMVP());

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
		dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

		renderProcess.createGraphicsPipelineLayout(dsLayouts, shaderManager.pushConstantRange);
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

	void recordGraphicsCommandBuffer(){
		ModelPushConstants pushConstants;
		// pushConstants.model = glm::mat4(1, 0, 0, 0,
		// 							    0, 1, 0, 0,
		// 								0 ,0, 1, 0,
		// 								0, 0, 0, 1);
		pushConstants.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		renderer.PushConstantToCommand<ModelPushConstants>(pushConstants, renderProcess.graphicsPipelineLayout, shaderManager.pushConstantRange);

		//drawObject(0);
		object.RecordDrawIndexCmd();
	}

	// void drawObject(int objectId){
	// 	std::vector<std::vector<VkDescriptorSet>> dsSets; 
	// 	dsSets.push_back(graphicsDescriptorManager.descriptorSets);
	// 	dsSets.push_back(object.descriptorSets); //set = 1

	// 	renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, dsSets, objectId);
	// 	renderer.BindVertexBuffer(objectId);
	// 	renderer.BindIndexBuffer(objectId);
	// 	renderer.DrawIndexed(triangleObject.indices3D);
	// }
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif