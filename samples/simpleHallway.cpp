#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleHallway
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D;
	std::vector<uint32_t> indices3D;

    void initialize(){
		swapchain.swapchainImage.bEnableMSAA = true;//!To enable MSAA, make sure it has depth test first (call wxjCreateDepthAttachment())
		textureImage.bEnableMipMap = true;

		mainCamera.type = Camera::CameraType::firstperson;
		mainCamera.setPosition(glm::vec3(0.0f, -0.8f, 0.0f));
		mainCamera.setRotation(glm::vec3(0.0f, 90.00001f, 0.0f));
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		//Create vertex resource
		wxjLoadObjModel("../models/hallway.obj", vertices3D, indices3D);

		//Create buffers
		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);
		renderer.InitCreateCommandPool(surface);
		renderer.InitCreateCommandBuffers();

		//Create texture resource
		VkImageUsageFlags usage_texture = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		//wxjCreateImage_texture("../textures/checkerboard_marble.jpg", usage_texture, textureImageBuffer, texWidth, texHeight); //if bEnableMipmap == true, update mipLevels here
		//wxjCreateImageView(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, OUT textureImageView);
		textureImage.CreateImage_texture("../textures/checkerboard_marble.jpg", usage_texture, textureImage.textureImageBuffer, textureImage.texWidth, textureImage.texHeight, renderer.commandPool);
		textureImage.textureImageView = textureImage.createImageView(textureImage.textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, textureImage.mipLevels);


		if(textureImage.mipLevels > 1) {
			//wxjCreateMipmaps(textureImage.textureImageBuffer.image); //, usage_texture,"../textures/checkerboard" 
			//textureImage.generateMipmaps(textureImage.textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, textureImage.texWidth, textureImage.texHeight, textureImage.mipLevels, NULL, false, renderer.commandPool);
			textureImage.CreateRainbowMipmaps(textureImage.textureImageBuffer.image, usage_texture,"../textures/checkerboard", renderer.commandPool);
		}

		wxjCreateSwapChainImagesAndImageViews();

		VkImageUsageFlags usage;
		//Create msaa resource
		if(swapchain.swapchainImage.bEnableMSAA){
			//wxjGetMaxUsableSampleCount();
			swapchain.swapchainImage.GetMaxUsableSampleCount();
			usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			//wxjCreateImage(msaaSamples, swapchain.swapChainImageFormat, usage, OUT msaaColorImageBuffer);//need swapChainExtent. call this after swapchain creation
			//wxjCreateImageView(msaaColorImageBuffer.image, swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, OUT msaaColorImageView);
			swapchain.swapchainImage.createImage(swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, 1, swapchain.swapchainImage.msaaSamples, swapchain.swapChainImageFormat, 
				VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, OUT swapchain.swapchainImage.msaaColorImageBuffer);
			swapchain.swapchainImage.msaaColorImageView = swapchain.swapchainImage.createImageView(swapchain.swapchainImage.msaaColorImageBuffer.image, swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

		}

		//Create depth resource
		swapchain.swapchainImage.bEnableDepthTest = true; //TODO: for clean up only
		VkFormat depthFormat = renderProcess.findDepthFormat();
		usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		//wxjCreateImage(msaaSamples, depthFormat, usage, OUT depthImageBuffer);//need swapChainExtent. call this after swapchain creation
		//wxjCreateImageView(depthImageBuffer.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, OUT depthImageView);
		swapchain.swapchainImage.createImage(swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, 1, swapchain.swapchainImage.msaaSamples, depthFormat, 
				VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, OUT swapchain.swapchainImage.depthImageBuffer);
		swapchain.swapchainImage.depthImageView = swapchain.swapchainImage.createImageView(swapchain.swapchainImage.depthImageBuffer.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);


		//Create Renderpass
		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		if(swapchain.swapchainImage.bEnableMSAA) imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat, swapchain.swapchainImage.msaaSamples, imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.addDepthAttachment(); //add this function will enable depth attachment(bUseDepthAttachment = true)
		if(swapchain.swapchainImage.bEnableMSAA) renderProcess.addColorAttachmentResolve(); //add this function will enable color resolve attachment (bUseColorAttachmentResolve = true)
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
		descriptor.addImageSamplerUniformBuffer(textureImage.mipLevels);
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(&textureImage.textureImageView);

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
		renderer.BeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  },  { 1.0f, 0 } };
		renderer.BeginRenderPass(renderProcess.renderPass, swapchain.swapChainFramebuffers, swapchain.swapChainExtent, clearValues);

		renderer.BindPipeline(renderProcess.graphicsPipeline);
		renderer.SetViewport(swapchain.swapChainExtent);
		renderer.SetScissor(swapchain.swapChainExtent);
		renderer.BindVertexBuffer();
		renderer.BindIndexBuffer();
		renderer.BindDescriptorSets(renderProcess.pipelineLayout, descriptor.descriptorSets);
		renderer.DrawIndexed(indices3D);

		renderer.EndRenderPass();
		renderer.EndCOmmandBuffer();

		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"