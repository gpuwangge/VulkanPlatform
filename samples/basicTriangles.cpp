#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CBasicTriangles
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	struct CustomUniformBufferObject {
		glm::vec3 color;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

	CObject triangleObject;

	void initialize(){
		triangleObject.InitVertices3D(vertices3D);
		triangleObject.InitIndices3D(indices3D);

		renderer.CreateVertexBuffer<Vertex3D>(triangleObject.vertices3D);
		renderer.CreateIndexBuffer(triangleObject.indices3D);

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("texture.jpg", usage, renderer.commandPool);	
		
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("basicTriangles/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("basicTriangles/frag.spv", shaderManager.FRAG);

		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);
		CGraphicsDescriptorManager::addCustomUniformBuffer(sizeof(CustomUniformBufferObject));
		CDescriptorManager::createDescriptorPool(); //pool size = 3
		VkDescriptorSetLayoutBinding customBinding = CustomUniformBufferObject::GetBinding();
		CGraphicsDescriptorManager::createDescriptorSetLayout(&customBinding); //layout size = 2 (exclude sampler)
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); //layout size = 1

		graphicsDescriptorManager.createDescriptorSets();
		triangleObject.createTextureDescriptorSets(
			textureManager.textureImages[triangleObject.id], 
			CGraphicsDescriptorManager::descriptorPool,
			CGraphicsDescriptorManager::textureDescriptorSetLayout,
			CGraphicsDescriptorManager::textureSamplers[0]);

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
		customUBO.color = {(sin(durationTime) + 1.0f) / 2.0f, 0.0f, (cos(durationTime) + 1.0f) / 2.0f};
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		drawObject(0);
	}

	void drawObject(int objectId){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		dsSets.push_back(graphicsDescriptorManager.descriptorSets);
		dsSets.push_back(triangleObject.descriptorSets); //set = 1

		renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, dsSets, objectId);
		renderer.BindVertexBuffer(objectId);
		renderer.BindIndexBuffer(objectId);
		renderer.DrawIndexed(indices3D);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

