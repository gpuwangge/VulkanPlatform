#include "..\\framework\\vulkanBase.h"
#define TEST_CLASS_NAME CModelMSAA
class TEST_CLASS_NAME: public CVulkanBase{
public:
    void initialize(){
		bEnableMSAA = true;//!To enable MSAA, make sure it has depth test first (call wxjCreateDepthAttachment())

		//Create vertex resource
		wxjLoadObjModel("../models/viking_room.obj");
		
		//Create buffers
		wxjCreateVertexBuffer();
		wxjCreateIndexBuffer();
		wxjCreateUniformBuffers();
		wxjCreateCommandBuffer();

		//Create texture resource
		wxjCreateImage_texture("../textures/viking_room.png", textureImageBuffer, texWidth, texHeight);
		wxjCreateImageView(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, OUT textureImageView);
		wxjCreateSampler_texture();

		wxjCreateSwapChainImagesAndImageViews();

		//Create msaa resource
		if(bEnableMSAA){
			wxjGetMaxUsableSampleCount();
			VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			wxjCreateImage(msaaSamples, swapChainImageFormat, usage, OUT msaaColorImageBuffer);//need swapChainExtent. call this after swapchain creation
			wxjCreateImageView(msaaColorImageBuffer.image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, OUT msaaColorImageView);
		}

		//Create depth resource
		VkFormat depthFormat = findDepthFormat();
		VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		wxjCreateImage(msaaSamples, depthFormat, usage, OUT depthImageBuffer);//need swapChainExtent. call this after swapchain creation
		wxjCreateImageView(depthImageBuffer.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, OUT depthImageView);

		//Create Renderpass
		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		if(bEnableMSAA) imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		wxjCreateColorAttachment(imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		wxjCreateDepthAttachment(); //add this function will enable depth attachment(bUseDepthAttachment = true)
		if(bEnableMSAA) wxjCreateColorAttachmentResolve(); //add this function will enable color resolve attachment (bUseColorAttachmentResolve = true)
		wxjCreateSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		wxjCreateDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		wxjCreateRenderPass();

		wxjCreateFramebuffers(); //need create imageviews first

		//Create shader resource
		wxjCreateVertexShader("../shaders/model/vert_model.spv");
		wxjCreateFragmentShader("../shaders/model/frag_model.spv");

		//Create Descriptors
		std::vector<VkDescriptorType> descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
		std::vector<VkShaderStageFlagBits> shaderStageFlagBits{VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
		wxjCreateDescriptorPool(descriptorTypes);
		wxjCreateDescriptorSetLayout(descriptorTypes, shaderStageFlagBits);
		wxjCreateDescriptorSets(descriptorTypes);

		wxjCreateGraphicsPipeline(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST); //need all above completed first

		CApplication::initialize();
	}

	void update(){
		ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CApplication::update();
	}

	void recordCommandBuffer(){
		wxjBeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  1.0f, 1.0f, 1.0f, 1.0f  },  { 1.0f, 0 } };
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

#include "..\\framework\\main.hpp"