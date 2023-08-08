#include "..\\framework\\include\\vulkanBase.h"

#define TEST_CLASS_NAME CSimpleUniformBuffer
class TEST_CLASS_NAME: public CVulkanBase{
public:
	struct CustomUniformBufferObject {
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
	CustomUniformBufferObject customUBO{};

	void initialize(){
		//Create bufferss
		wxjCreateCommandBuffer();

		wxjCreateSwapChainImagesAndImageViews();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		wxjCreateFramebuffers();

		//Create shader resources
		shaderManager.InitVertexShader("../shaders/simpleUniformBuffer/vert.spv");
		shaderManager.InitFragmentShader("../shaders/simpleUniformBuffer/frag.spv");

		//Create Descriptors
		descriptor.addCustomUniformBuffer(sizeof(CustomUniformBufferObject));
		descriptor.createDescriptorPool();
		VkDescriptorSetLayoutBinding customBinding = CustomUniformBufferObject::GetBinding();
		descriptor.createDescriptorSetLayout(&customBinding);
		descriptor.createDescriptorSets();

		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule, 
			descriptor.descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		//printf("%f\n", durationTime);
		customUBO.color = {(sin(durationTime) + 1.0f) / 2.0f, (cos(durationTime) + 1.0f) / 2.0f, 0.0f, 1.0f};
		descriptor.updateCustomUniformBuffer<CustomUniformBufferObject>(currentFrame, durationTime, customUBO);
		CApplication::update();
	}

	void recordCommandBuffer(){
		wxjBeginCommandBuffer();
		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
		wxjBeginRenderPass(clearValues);
		wxjBindPipeline();
		wxjSetViewport();
		wxjSetScissor();
		wxjBindDescriptorSets();
		wxjDraw(3);
		wxjEndRenderPass();
		wxjEndCOmmandBuffer();
		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"


