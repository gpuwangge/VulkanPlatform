#include "..\\framework\\include\\vulkanBase.h"
#define TEST_CLASS_NAME CSimpleHallway
class TEST_CLASS_NAME: public CVulkanBase{
public:
	std::vector<Vertex3D> vertices3D;
	std::vector<uint32_t> indices3D;

    void initialize(){
		bEnableMSAA = true;//!To enable MSAA, make sure it has depth test first (call wxjCreateDepthAttachment())
		bEnableMipMap = true;

		mainCamera.type = Camera::CameraType::firstperson;
		mainCamera.setPosition(glm::vec3(0.0f, -0.8f, 0.0f));
		mainCamera.setRotation(glm::vec3(0.0f, 90.00001f, 0.0f));
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		//Create vertex resource
		wxjLoadObjModel("../models/hallway.obj", vertices3D, indices3D);

		//Create buffers
		wxjCreateVertexBuffer<Vertex3D>(vertices3D);
		wxjCreateIndexBuffer(indices3D);
		wxjCreateCommandBuffer();

		//Create texture resource
		VkImageUsageFlags usage_texture = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		wxjCreateImage_texture("../textures/checkerboard_marble.jpg", usage_texture, textureImageBuffer, texWidth, texHeight); //if bEnableMipmap == true, update mipLevels here
		wxjCreateImageView(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, OUT textureImageView);

		if(mipLevels > 1) wxjCreateMipmaps(textureImageBuffer.image); //, usage_texture,"../textures/checkerboard"

		wxjCreateSwapChainImagesAndImageViews();

		VkImageUsageFlags usage;
		//Create msaa resource
		if(bEnableMSAA){
			wxjGetMaxUsableSampleCount();
			usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			wxjCreateImage(msaaSamples, swapchain.swapChainImageFormat, usage, OUT msaaColorImageBuffer);//need swapChainExtent. call this after swapchain creation
			wxjCreateImageView(msaaColorImageBuffer.image, swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, OUT msaaColorImageView);
		}

		//Create depth resource
		VkFormat depthFormat = renderProcess.findDepthFormat();
		usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		wxjCreateImage(msaaSamples, depthFormat, usage, OUT depthImageBuffer);//need swapChainExtent. call this after swapchain creation
		wxjCreateImageView(depthImageBuffer.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, OUT depthImageView);

		//Create Renderpass
		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		if(bEnableMSAA) imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat, msaaSamples, imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.addDepthAttachment(); //add this function will enable depth attachment(bUseDepthAttachment = true)
		if(bEnableMSAA) renderProcess.addColorAttachmentResolve(); //add this function will enable color resolve attachment (bUseColorAttachmentResolve = true)
		renderProcess.createSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		renderProcess.createRenderPass();

		wxjCreateFramebuffers(); //need create imageviews first

		//Create shader resource
		shaderManager.InitVertexShader("../shaders/model/vert_model.spv");
		shaderManager.InitFragmentShader("../shaders/model/frag_model.spv");

		//Create Descriptors
		descriptor.addMVPUniformBuffer();
		descriptor.addImageSamplerUniformBuffer(mipLevels);
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(&textureImageView);

		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule, 
			descriptor.descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();

		//static int counter = 0;
		//if(counter==0)NeedToExit = true;
		//counter++;
	}

	void recordCommandBuffer(){
		wxjBeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  },  { 1.0f, 0 } };
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

#include "..\\framework\\include\\main.hpp"