#include "..\\framework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleTexture
#include "object.h"
#include "supervisor.h"
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};

	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	CObject object;

	std::vector<std::pair<std::string, bool>> textureNames = {{"texture.jpg", false}}; //first: textureName, second: mipmap
	std::string vertexShader = "simpleTexture/vert.spv";
	std::string fragmentShader = "simpleTexture/frag.spv";

    void initialize(){
		mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    	mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    	mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);

		CMastermind::Register((CApplication*)this);
		CMastermind::VertexShader = vertexShader;
		CMastermind::FragmentShader = fragmentShader;
		CMastermind::Activate_Uniform_Graphics_MVP();
		CMastermind::Activate_Uniform_Graphics_Sampler();
		CMastermind::Activate_Buffer_Graphics_Vertex(VertexStructureTypes::ThreeDimension);
		CMastermind::LoadResources(vertices3D, indices3D, &textureNames);


		/*
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureManager.CreateTextureImage("texture.jpg", usage, renderer.commandPool);

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleTexture/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleTexture/frag.spv", shaderManager.FRAG); 

		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);
		CDescriptorManager::createDescriptorPool(); //pool size = 2

		CGraphicsDescriptorManager::createDescriptorSetLayout(); //layout size = 1
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); //layout size = 1

		graphicsDescriptorManager.createDescriptorSets();
		*/

		object.Register((CApplication*)this); 

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		// std::vector<VkDescriptorSetLayout> dsLayouts;
		// dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout); //set = 0
		// dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

		// renderProcess.createGraphicsPipelineLayout(dsLayouts);
		// renderProcess.createGraphicsPipeline<Vertex3D>(
		// 	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
		// 	shaderManager.vertShaderModule, 
		// 	shaderManager.fragShaderModule);
		
		CApplication::initialize();
	}

	void update(){
		//descriptors[0].mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//graphicsDescriptorManager.mvpUBO.mvpData[0].model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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