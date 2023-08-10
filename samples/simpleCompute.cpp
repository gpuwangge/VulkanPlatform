#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleCompute
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	void initialize(){
		//0. prepare compute shader
		//1. need a physical device with compute, and pick Queue Family with compute
		//2. create layouts for compute shader
		//3. create storage buffer
		//4. create compute pipeline (with shader and layout information)
		//5. record command (bind compute shader and dispatch)
		
		renderer.CreateCommandPool(surface);
		renderer.CreateCommandBuffers();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.InitVertexShader("../shaders/simpleTriangle/vert.spv");
		shaderManager.InitFragmentShader("../shaders/simpleTriangle/frag.spv");

		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets();

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

	void recordCommandBuffer(){
		RENDER_START
		//actually this sample doesn't need BindDescriptorSets

		renderer.Draw(3);
		
		RENDER_END
	}
};

#include "..\\framework\\include\\main.hpp"


