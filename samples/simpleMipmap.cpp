#include "..\\framework\\include\\application.h"
#include "object.h"
#define TEST_CLASS_NAME CSimpleMipmap
class TEST_CLASS_NAME: public CApplication{
public:
	CObject object;
    void initialize(){
		mainCamera.type = Camera::CameraType::firstperson;
		mainCamera.setPosition(glm::vec3(0.0f, -0.8f, 0.0f));
		mainCamera.setRotation(glm::vec3(0.0f, 90.00001f, 0.0f));
		mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		swapchain.EnableMSAA();

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage_texture = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("checkerboard_marble.jpg", usage_texture, renderer.commandPool, true);
		textureManager.textureImages[0].generateMipmaps("checkerboard", usage_texture); //mix mipmap: "checkerboard", usage_texture

		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat, swapchain.bEnableDepthTest, swapchain.depthFormat, swapchain.msaaSamples, imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleMipmap/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleMipmap/frag.spv", shaderManager.FRAG);

		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);

		CDescriptorManager::createDescriptorPool();

		CGraphicsDescriptorManager::createDescriptorSetLayout();
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout();

		graphicsDescriptorManager.createDescriptorSets();
		object.Register((CApplication*)this, "hallway.obj");

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
		dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

		renderProcess.createGraphicsPipelineLayout(dsLayouts);
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
		object.Draw();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif