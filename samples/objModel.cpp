#include "..\\framework\\vulkanBase.h"
#define TEST_CLASS_NAME CObjModel
class TEST_CLASS_NAME: public CVulkanBase{
public:
    TEST_CLASS_NAME(){
    }

    ~TEST_CLASS_NAME(){
    }

    void initialize(){
		wxjLoadObjModel("../models/viking_room.obj");

		wxjCreateVertexBuffer();
		wxjCreateIndexBuffer();
		wxjCreateUniformBuffers();
		wxjCreateCommandBuffer();

		wxjCreateImage_texture("../textures/viking_room.png", textureImageBuffer, texWidth, texHeight);
		wxjCreateSampler_texture();
		wxjCreateImageView(textureImageBuffer.image);

		wxjCreateSwapChain();

		wxjCreateColorAttachment();
		//wxjCreatDepthAttachment();
		//wxjCreatColorAttachmentResolve();
		wxjCreateSubpass();
		wxjCreateDependency();
		wxjCreateRenderPass();

		wxjCreateFramebuffers();

		wxjCreateVertexShader("../shaders/model/vert_model.spv");
		wxjCreateFragmentShader("../shaders/model/frag_model.spv");

		std::vector<VkDescriptorType> descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
		std::vector<VkShaderStageFlagBits> shaderStageFlagBits{VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
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

		std::vector<VkClearValue> clearValues{ {  1.0f, 1.0f, 1.0f, 1.0f  } };
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