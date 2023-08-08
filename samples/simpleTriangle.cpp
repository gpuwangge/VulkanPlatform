#include "..\\framework\\include\\vulkanBase.h"

#define TEST_CLASS_NAME CSimpleTriangle
class TEST_CLASS_NAME: public CVulkanBase{
public:
	void initialize(){
		//Create bufferss
		renderer.InitCreateCommandPool(surface);
		renderer.InitCreateCommandBuffers();

		wxjCreateSwapChainImagesAndImageViews();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		wxjCreateFramebuffers();

		//Create shader resource
		shaderManager.InitVertexShader("../shaders/simpleTriangle/vert.spv");
		shaderManager.InitFragmentShader("../shaders/simpleTriangle/frag.spv");

		//Create Descriptors
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets();

		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule, 
			descriptor.descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordCommandBuffer(){
		renderer.BeginCommandBuffer();
		std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };
		renderer.BeginRenderPass(renderProcess.renderPass, swapchain.swapChainFramebuffers, swapchain.swapChainExtent, clearValues);
		renderer.BindPipeline(renderProcess.graphicsPipeline);
		renderer.SetViewport(swapchain.swapChainExtent);
		renderer.SetScissor(swapchain.swapChainExtent);
		renderer.Draw(3);
		renderer.EndRenderPass();
		renderer.EndCOmmandBuffer();
		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"


