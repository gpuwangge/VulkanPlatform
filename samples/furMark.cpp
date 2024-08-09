#include "..\\framework\\include\\application.h"
#include "object.h"
#define TEST_CLASS_NAME CFurMark
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
 
	struct CustomUniformBufferObject {
		//glm::vec3 color;
        glm::vec2 u_resolution;
	    float u_time;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUBO{};

	CObject object;

	void initialize(){
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("fur.jpg", usage, renderer.commandPool);	
		textureManager.CreateTextureImage("noise.png", usage, renderer.commandPool);	
		textureManager.CreateTextureImage("wall.jpg", usage, renderer.commandPool);	

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("furMark/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("furMark/frag.spv", shaderManager.FRAG);

		//Uniform List:
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(textureManager.textureImages[0].mipLevels);
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(textureManager.textureImages[1].mipLevels);
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(textureManager.textureImages[2].mipLevels);
		CGraphicsDescriptorManager::addCustomUniformBuffer(sizeof(CustomUniformBufferObject));

		CDescriptorManager::createDescriptorPool(); 

		VkDescriptorSetLayoutBinding customBinding = CustomUniformBufferObject::GetBinding();
		CGraphicsDescriptorManager::createDescriptorSetLayout(&customBinding); 
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); 

		graphicsDescriptorManager.createDescriptorSets();

		object.Register((CApplication*)this, vertices3D, indices3D, 0, true);

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
		//customUBO.color = {(sin(durationTime) + 1.0f) / 2.0f, 0.0f, (cos(durationTime) + 1.0f) / 2.0f};
        customUBO.u_time = durationTime;
        customUBO.u_resolution = glm::vec2(windowWidth, windowHeight);
		graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);

		//descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw(); 
	}

};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

