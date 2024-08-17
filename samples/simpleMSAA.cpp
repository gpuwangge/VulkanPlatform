#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CSimpleMSAA
class TEST_CLASS_NAME: public CApplication{
public:
	CObject object;

	std::vector<std::string> modelNames = {"viking_room.obj"}; 
	std::vector<std::pair<std::string, bool>> textureNames = {{"viking_room.png", false}}; //first: textureName, second: mipmap
	std::string vertexShader = "simpleMSAA/vert.spv";
	std::string fragmentShader = "simpleMSAA/frag.spv";	

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
		
		CMastermind::Register((CApplication*)this);
		CMastermind::VertexShader = vertexShader;
		CMastermind::FragmentShader = fragmentShader;
		CMastermind::Activate_Uniform_Graphics_MVP();
		CMastermind::Activate_Feature_Graphics_MSAA();
		CMastermind::Activate_Uniform_Graphics_Sampler();
		CMastermind::LoadResources(modelNames, &textureNames);

		/*
		swapchain.EnableMSAA();
		//swapchain.EnableDepthTest();

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("viking_room.png", usage, renderer.commandPool);

		VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat, swapchain.bEnableDepthTest, swapchain.depthFormat, swapchain.msaaSamples, imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleMSAA/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleMSAA/frag.spv", shaderManager.FRAG);


		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);

		CDescriptorManager::createDescriptorPool();

		CGraphicsDescriptorManager::createDescriptorSetLayout();
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout();

		graphicsDescriptorManager.createDescriptorSets();
		*/

	
		object.Register((CApplication*)this);

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		// std::vector<VkDescriptorSetLayout> dsLayouts;
		// dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
		// dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

		// renderProcess.createGraphicsPipelineLayout(dsLayouts);
		// renderProcess.createGraphicsPipeline<Vertex3D>(
		// 	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
		// 	shaderManager.vertShaderModule, 
		// 	shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		//CGraphicsDescriptorManager::mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		object.SetRotation(glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		object.Draw();
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif