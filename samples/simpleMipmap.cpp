#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleMipmap
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D;
	std::vector<uint32_t> indices3D;

	std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  },  { 1.0f, 0 } };

    void initialize(){
		swapchain.bEnableDepthTest = true;
		swapchain.bEnableMSAA = true; //!To enable MSAA, make sure it has depth test first (call wxjCreateDepthAttachment())
		textureImages[0].bEnableMipMap = true;

		mainCamera.type = Camera::CameraType::firstperson;
		mainCamera.setPosition(glm::vec3(0.0f, -0.8f, 0.0f));
		mainCamera.setRotation(glm::vec3(0.0f, 90.00001f, 0.0f));
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		modelManager.LoadObjModel("hallway.obj", vertices3D, indices3D);

		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage_texture = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureImages[0].CreateTextureImage("checkerboard_marble.jpg", usage_texture, renderer.commandPool);
		textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		//textureImages[0].generateMipmaps();//normal mipmap 
		textureImages[0].generateMipmaps("checkerboard", usage_texture); //mix mipmap
		
		VkImageUsageFlags usage;
		if(swapchain.bEnableMSAA){
			swapchain.GetMaxUsableSampleCount();
			usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchain.createMSAAImages(VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			swapchain.createMSAAImageViews(VK_IMAGE_ASPECT_COLOR_BIT);
		}

		VkFormat depthFormat = renderProcess.findDepthFormat();
		usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		swapchain.createDepthImages(depthFormat, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		swapchain.createDepthImageViews(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		if(swapchain.bEnableMSAA) imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat, swapchain.msaaSamples, imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.addDepthAttachment(); //add this function will enable depth attachment(bUseDepthAttachment = true)
		if(swapchain.bEnableMSAA) renderProcess.addColorAttachmentResolve(); //add this function will enable color resolve attachment (bUseColorAttachmentResolve = true)
		renderProcess.createSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.CreateVertexShader("simpleObjLoader/vert.spv");
		//shaderManager.CreateFragmentShader("simpleObjLoader/frag.spv");
		shaderManager.CreateShader("simpleObjLoader/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleObjLoader/frag.spv", shaderManager.FRAG);

		descriptors[0].addMVPUniformBuffer();
		descriptors[0].addImageSamplerUniformBuffer(textureImages[0].mipLevels);
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets(&textureImages);

		renderProcess.createGraphicsPipelineLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
		//static int counter = 0;
		//if(counter==0)NeedToExit = true;
		//counter++;
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)

		renderer.BindVertexBuffer();
		renderer.BindIndexBuffer();
		renderer.DrawIndexed(indices3D);

		END_GRAPHICS_RECORD
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif