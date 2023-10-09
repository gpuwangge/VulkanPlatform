#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleTriangle
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	void initialize(){
		renderer.CreateCommandPool(surface);
		renderer.CreateCommandBuffers();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.CreateVertexShader("simpleTriangle/vert.spv");
		//shaderManager.CreateFragmentShader("simpleTriangle/frag.spv");
		shaderManager.CreateShader("simpleTriangle/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleTriangle/frag.spv", shaderManager.FRAG); 

		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets(textureImages);

		renderProcess.createLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)
		//actually this sample doesn't need BindDescriptorSets

		renderer.Draw(3);
		
		END_GRAPHICS_RECORD
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

