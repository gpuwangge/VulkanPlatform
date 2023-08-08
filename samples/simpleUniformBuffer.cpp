#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleUniformBuffer
class TEST_CLASS_NAME: public CApplication{
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
		renderer.InitCreateCommandPool(surface);
		renderer.InitCreateCommandBuffers();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		//wxjCreateFramebuffers();
		swapchain.CreateFramebuffers(renderProcess.bUseDepthAttachment, renderProcess.bUseColorAttachmentResolve, renderProcess.renderPass);

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
		descriptor.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);
		CApplication::update();
	}

	void recordCommandBuffer(){
		renderer.BeginCommandBuffer();
		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
		renderer.BeginRenderPass(renderProcess.renderPass, swapchain.swapChainFramebuffers, swapchain.swapChainExtent, clearValues);
		renderer.BindPipeline(renderProcess.graphicsPipeline);
		renderer.SetViewport(swapchain.swapChainExtent);
		renderer.SetScissor(swapchain.swapChainExtent);
		renderer.BindDescriptorSets(renderProcess.pipelineLayout, descriptor.descriptorSets);
		renderer.Draw(3);
		renderer.EndRenderPass();
		renderer.EndCOmmandBuffer();
		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"


