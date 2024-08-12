#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CBptpc16Texture
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	CObject object;

	std::vector<std::pair<std::string, bool>> textureNames = {{"48bpt.png", true}}; //first: textureName, second: mipmap
	std::string vertexShader = "bptpc16Texture/vert.spv";
	std::string fragmentShader = "bptpc16Texture/frag.spv";

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
		mainCamera.setPerspective(90.0f, (float)windowWidth /  (float)windowHeight, 0.1f, 256.0f);

		CMastermind::Register((CApplication*)this);
		CMastermind::VertexShader = vertexShader;
		CMastermind::FragmentShader = fragmentShader;
		CMastermind::ActivateSampler();
		CMastermind::ActivateMVP();
		CMastermind::ActivateMSAA();
		CMastermind::Activate48BPT();
		CMastermind::LoadResources(vertices3D, indices3D, &textureNames);

		/*
		swapchain.EnableMSAA();

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage_texture = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("48bpt.png", usage_texture, renderer.commandPool, true, VK_FORMAT_R16G16B16A16_UNORM, 16);
		textureManager.textureImages[0].generateMipmaps();

		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat, swapchain.bEnableDepthTest, swapchain.depthFormat, swapchain.msaaSamples, imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("bptpc16Texture/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("bptpc16Texture/frag.spv", shaderManager.FRAG);

		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);
		
		CDescriptorManager::createDescriptorPool(); //pool size = 2

		CGraphicsDescriptorManager::createDescriptorSetLayout(); //layout size = 1 (exclude sampler)
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); //layout size = 1

		graphicsDescriptorManager.createDescriptorSets();
		*/



		object.Register((CApplication*)this); //, vertices3D, indices3D

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
	}

	void recordGraphicsCommandBuffer(){
		object.Draw();
	}

};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif