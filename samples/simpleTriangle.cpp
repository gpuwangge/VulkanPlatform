#include "..\\framework\\vulkanBase.h"

#define TEST_CLASS_NAME CSimpleTriangle
class TEST_CLASS_NAME: public CVulkanBase{
public:
	void initialize(){
		//Create bufferss
		wxjCreateCommandBuffer();

		wxjCreateSwapChainImagesAndImageViews();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		wxjCreateFramebuffers();

		//Create shader resource
		wxjCreateVertexShader("../shaders/simpleTriangle/vert.spv");
		wxjCreateFragmentShader("../shaders/simpleTriangle/frag.spv");

		//Create Descriptors
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets();

		renderProcess.createGraphicsPipeline(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, vertShaderModule, fragShaderModule, descriptor.descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordCommandBuffer(){
		wxjBeginCommandBuffer();
		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
		wxjBeginRenderPass(clearValues);
		wxjBindPipeline();
		wxjSetViewport();
		wxjSetScissor();
		wxjDraw(3);
		wxjEndRenderPass();
		wxjEndCOmmandBuffer();
		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\main.hpp"


