#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleUniformBuffer
class TEST_CLASS_NAME: public CApplication{
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

	std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };

	void initialize(){
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.CreateVertexShader("simpleUniformBuffer/vert.spv");
		//shaderManager.CreateFragmentShader("simpleUniformBuffer/frag.spv");
		shaderManager.CreateShader("simpleUniformBuffer/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleUniformBuffer/frag.spv", shaderManager.FRAG); 

		descriptors[0].addCustomUniformBuffer(sizeof(StructCustomUniformBuffer));
		descriptors[0].createDescriptorPool();
		VkDescriptorSetLayoutBinding customBinding = StructCustomUniformBuffer::GetBinding();
		descriptors[0].createDescriptorSetLayout(&customBinding);
		descriptors[0].createDescriptorSets();

		renderProcess.createGraphicsPipelineLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		//printf("%f\n", durationTime);
		customUniformBufferObject.color = {(sin(durationTime*3) + 1.0f) / 2.0f, (cos(durationTime*3) + 1.0f) / 2.0f, 0.0f, 1.0f};
		descriptors[0].updateCustomUniformBuffer<StructCustomUniformBuffer>(renderer.currentFrame, durationTime, customUniformBufferObject);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)

		drawObject(0);
		
		END_GRAPHICS_RECORD
	}

	void drawObject(int objectId){
		renderer.BindGraphicsDescriptorSets(renderProcess.graphicsPipelineLayout, descriptors[0].descriptorSets, -1);
		//renderer.BindVertexBuffer(objectId);
		//renderer.BindIndexBuffer(objectId);
		renderer.Draw(3);
	}	
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

