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

	std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };

	void initialize(){
		renderer.CreateCommandPool(surface);
		renderer.CreateCommandBuffers();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.InitVertexShader("../shaders/simpleUniformBuffer/vert.spv");
		shaderManager.InitFragmentShader("../shaders/simpleUniformBuffer/frag.spv");

		descriptor.addCustomUniformBuffer(sizeof(CustomUniformBufferObject));
		descriptor.createDescriptorPool();
		VkDescriptorSetLayoutBinding customBinding = CustomUniformBufferObject::GetBinding();
		descriptor.createDescriptorSetLayout(&customBinding);
		descriptor.createDescriptorSets();

		renderProcess.createLayout(descriptor.descriptorSetLayout);
		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		//printf("%f\n", durationTime);
		customUBO.color = {(sin(durationTime*3) + 1.0f) / 2.0f, (cos(durationTime*3) + 1.0f) / 2.0f, 0.0f, 1.0f};
		descriptor.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, durationTime, customUBO);
		CApplication::update();
	}

	void recordCommandBuffer(){
		RENDER_START

		renderer.Draw(3);
		
		RENDER_END
	}
};

#include "..\\framework\\include\\main.hpp"


