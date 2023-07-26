#include "..\\framework\\vulkanBase.h"

#define TEST_CLASS_NAME CSimpleTriangle
class TEST_CLASS_NAME: public CVulkanBase{
public:
    TEST_CLASS_NAME(){
		/*****
		 * Other things to prepare
		 * Renderpass: colorAttachment, depthAttachment(for model), colorAttachmentResolve(for MSAA)
		 * Framebuffer: swapChainImageViews[i], depthImageView(for model), colorImageView_msaa(for MSAA)
		 * Descriptor: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER (for MVP),VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER (for texture), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER (for compute shader)
		 * ****/
    }

    ~TEST_CLASS_NAME(){
    }

	void initialize(){
		/************
		buffersize: 8(numbers each vertex)*4(float)*4(vertex size)=128（byte）
		************/
		vertices3D = {
			{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
			{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
			{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
			{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
			};

		/************
		buffer size: 6*4=24（byte）
		************/
		indices3D = { 0, 1, 2, 2, 3, 0};

		wxjCreateVertexBuffer();
		wxjCreateIndexBuffer();
		wxjCreateUniformBuffers();
		wxjCreateCommandBuffer();

		wxjCreateSwapChain();

		wxjCreateColorAttachment();
		//wxjCreatDepthAttachment();
		//wxjCreatColorAttachmentResolve();
		wxjCreateSubpass();
		wxjCreateDependency();
		wxjCreateRenderPass();

		wxjCreateFramebuffers();

		wxjCreateVertexShader("../shaders/basic/vert.spv");
		wxjCreateFragmentShader("../shaders/basic/frag.spv");

		std::vector<VkDescriptorType> descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
		std::vector<VkShaderStageFlagBits> shaderStageFlagBits{VK_SHADER_STAGE_VERTEX_BIT};
		wxjCreateDescriptorPool(descriptorTypes);
		wxjCreateDescriptorSetLayout(descriptorTypes, shaderStageFlagBits);
		wxjCreateDescriptorSets(descriptorTypes);

		wxjCreateGraphicsPipeline(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		wxjCreateSyncObjects();

		CApplication::initialize();
	}

	void update(){
		ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordCommandBuffer(){
		wxjBeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
		wxjBeginRenderPass(clearValues);

		wxjBindPipeline();
		wxjSetViewport();
		wxjSetScissor();
		wxjBindVertexBuffer();
		wxjBindIndexBuffer();
		wxjBindDescriptorSets();
		wxjDrawIndexed();

		wxjEndRenderPass();
		wxjEndCOmmandBuffer();

		CApplication::recordCommandBuffer();
	}
};

#include "main.hpp"


