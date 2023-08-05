#include "..\\framework\\vulkanBase.h"
#define TEST_CLASS_NAME CSimpleTexture
class TEST_CLASS_NAME: public CVulkanBase{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
	};

	/************
	buffer size: 6*4=24（byte）
	************/
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

    void initialize(){
		//Create buffers
		wxjCreateVertexBuffer<Vertex3D>(vertices3D);
		wxjCreateIndexBuffer(indices3D);
		wxjCreateCommandBuffer();

		//Create texture resource
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		wxjCreateImage_texture("../textures/texture.jpg", usage, textureImageBuffer, texWidth, texHeight);
		wxjCreateImageView(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, OUT textureImageView);

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
		wxjCreateVertexShader("../shaders/texture/vert.spv");
		wxjCreateFragmentShader("../shaders/texture/frag.spv");

		//Create Descriptors
		descriptor.addMVPUniformBuffer();
		descriptor.addImageSamplerUniformBuffer(mipLevels);
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(&textureImageView);

		wxjCreateGraphicsPipeline<Vertex3D>(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		CApplication::initialize();
	}

	void update(){
		descriptor.mvpUBO.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
		wxjDrawIndexed(indices3D);

		wxjEndRenderPass();
		wxjEndCOmmandBuffer();

		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\main.hpp"