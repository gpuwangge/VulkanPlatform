#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimplePushConstant
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } }
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
	std::vector<VkClearValue> clearValues{ {  1.0f, 1.0f, 1.0f, 1.0f  } };
 
	CObject triangleObject;

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, 0.0f, -1.5f));

		triangleObject.InitVertices3D(vertices3D);
		triangleObject.InitIndices3D(indices3D);

		renderer.CreateVertexBuffer<Vertex3D>(triangleObject.vertices3D);
		renderer.CreateIndexBuffer(triangleObject.indices3D);
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		//textureImages[0].CreateTextureImage("texture.jpg", usage, renderer.commandPool);
		//textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
		textureManager.CreateTextureImage("texture.jpg", usage, renderer.commandPool);	

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.CreateVertexShader("simplePushConstant/vert.spv");
		//shaderManager.CreateFragmentShader("simplePushConstant/frag.spv");
		shaderManager.CreateShader("simplePushConstant/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simplePushConstant/frag.spv", shaderManager.FRAG); 
		
		shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);

		//descriptors[0].addVPUniformBuffer();
		//descriptors[0].addImageSamplerUniformBuffer(textureImages[0].mipLevels);
		//descriptors[0].createDescriptorPool();
		//descriptors[0].createDescriptorSetLayout();
		//descriptors[0].createDescriptorSets(&textureImages);
		CGraphicsDescriptorManager::addVPUniformBuffer();
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(textureManager.textureImages[0].mipLevels);
		CDescriptorManager::createDescriptorPool(); 
		CGraphicsDescriptorManager::createDescriptorSetLayout(); 
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); 

		graphicsDescriptorManager.createDescriptorSets();
		triangleObject.createTextureDescriptorSets(
			textureManager.textureImages[triangleObject.id], 
			CGraphicsDescriptorManager::descriptorPool,
			CGraphicsDescriptorManager::textureDescriptorSetLayout,
			CGraphicsDescriptorManager::textureSamplers[0]);

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		//dsLayouts.push_back(descriptors[0].descriptorSetLayout);
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
		START_GRAPHICS_RECORD(0)

		ModelPushConstants pushConstants;
		// pushConstants.model = glm::mat4(1, 0, 0, 0,
		// 							    0, 1, 0, 0,
		// 								0 ,0, 1, 0,
		// 								0, 0, 0, 1);
		pushConstants.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		renderer.PushConstantToCommand<ModelPushConstants>(pushConstants, renderProcess.graphicsPipelineLayout, shaderManager.pushConstantRange);

		drawObject(0);

		END_GRAPHICS_RECORD
	}

	void drawObject(int objectId){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		//dsSets.push_back(descriptors[0].descriptorSets);
		dsSets.push_back(graphicsDescriptorManager.descriptorSets);
		dsSets.push_back(triangleObject.descriptorSets); //set = 1

		renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, dsSets, objectId);
		renderer.BindVertexBuffer(objectId);
		renderer.BindIndexBuffer(objectId);
		renderer.DrawIndexed(triangleObject.indices3D);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif