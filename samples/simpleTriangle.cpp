#include "..\\framework\\include\\application.h"
#include "object.h"

#define TEST_CLASS_NAME CSimpleTriangle
class TEST_CLASS_NAME: public CApplication{
//a blue triangle on the screen
CObject object;
public:
	void initialize(){
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleTriangle/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleTriangle/frag.spv", shaderManager.FRAG); 

		//CDescriptorManager::createDescriptorPool(); 
		//CGraphicsDescriptorManager::createDescriptorSetLayout(); 
		//CGraphicsDescriptorManager::createTextureDescriptorSetLayout();
		//graphicsDescriptorManager.createDescriptorSets();

		object.Register((CApplication*)this);

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		//dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
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

