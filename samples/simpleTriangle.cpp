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

		shaderManager.CreateVertexShader("simpleTriangle/vert.spv");
		shaderManager.CreateFragmentShader("simpleTriangle/frag.spv");

		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(textureImages);

		renderProcess.createLayout(descriptor.descriptorSetLayout);
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
		RENDER_START
		//actually this sample doesn't need BindDescriptorSets

		renderer.Draw(3);
		
		RENDER_END
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

