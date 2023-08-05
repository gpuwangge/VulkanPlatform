#include "..\\framework\\vulkanBase.h"

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
		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		wxjCreateColorAttachment(imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		wxjCreateSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		wxjCreateDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		wxjCreateRenderPass();

		wxjCreateFramebuffers();

		//Create shader resource
		wxjCreateVertexShader("../shaders/simpleUniformBuffer/vert.spv");
		wxjCreateFragmentShader("../shaders/simpleUniformBuffer/frag.spv");

		//Create Descriptors
		descriptor.addCustomUniformBuffer(sizeof(CustomUniformBufferObject));
		descriptor.createDescriptorPool();
		VkDescriptorSetLayoutBinding bd = CustomUniformBufferObject::GetBinding();
		descriptor.createDescriptorSetLayout(bd.descriptorType, bd.stageFlags, bd.descriptorCount, bd.pImmutableSamplers);
		descriptor.createDescriptorSets();

		wxjCreateGraphicsPipeline(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

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

#include "..\\framework\\main.hpp"


