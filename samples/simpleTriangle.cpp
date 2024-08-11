#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CSimpleTriangle
class TEST_CLASS_NAME: public CApplication{
	//a blue triangle on the screen
	CObject object;

	std::vector<std::pair<std::string, bool>> textureNames = {}; //first: textureName, second: mipmap
	std::string vertexShader = "simpleTriangle/vert.spv";
	std::string fragmentShader = "simpleTriangle/frag.spv";

public:
	void initialize(){
		CSkyvision::Register((CApplication*)this);
		CSkyvision::LoadResources(textureNames, vertexShader, fragmentShader, 0); //0: no use sampler

		/*
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleTriangle/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleTriangle/frag.spv", shaderManager.FRAG); 
		*/
		
		object.Register((CApplication*)this, -1, -1, 0); //no texture, no model, id=0

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
		//dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout); //set = 0
		//dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1
		renderProcess.createGraphicsPipelineLayout(dsLayouts);
		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);
		
		CApplication::initialize();
	}

	void update(){
		//static int counter = 1;
		CApplication::update();

		//if(counter==7504) NeedToExit = true;
		//counter++;
	}

	void recordGraphicsCommandBuffer(){
		object.Draw(3);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

