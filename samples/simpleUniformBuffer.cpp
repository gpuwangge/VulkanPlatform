#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleUniformBuffer
class TEST_CLASS_NAME: public CApplication{
//a changeing color triangle on the screen
public:
	struct StructCustomUniformBuffer {
		glm::vec4 color;

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
	StructCustomUniformBuffer customUniformBufferObject{};

	void initialize(){
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleUniformBuffer/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleUniformBuffer/frag.spv", shaderManager.FRAG); 

		CGraphicsDescriptorManager::addCustomUniformBuffer(sizeof(StructCustomUniformBuffer));
		CDescriptorManager::createDescriptorPool();
		VkDescriptorSetLayoutBinding customBinding = StructCustomUniformBuffer::GetBinding();
		CGraphicsDescriptorManager::createDescriptorSetLayout(&customBinding);

		graphicsDescriptorManager.createDescriptorSets();

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);

		renderProcess.createGraphicsPipelineLayout(dsLayouts);
		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		//printf("%f\n", durationTime);
		customUniformBufferObject.color = {(sin(durationTime*3) + 1.0f) / 2.0f, (cos(durationTime*3) + 1.0f) / 2.0f, 0.0f, 1.0f};
		graphicsDescriptorManager.updateCustomUniformBuffer<StructCustomUniformBuffer>(renderer.currentFrame, durationTime, customUniformBufferObject);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		drawObject(0);
	}

	void drawObject(int objectId){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		dsSets.push_back(graphicsDescriptorManager.descriptorSets);

		renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, dsSets, -1);
		//renderer.BindVertexBuffer(objectId);
		//renderer.BindIndexBuffer(objectId);
		renderer.Draw(3);
	}	
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

