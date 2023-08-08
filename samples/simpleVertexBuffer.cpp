#include "..\\framework\\include\\vulkanBase.h"

#define TEST_CLASS_NAME CSimpleVertexBuffer
class TEST_CLASS_NAME: public CVulkanBase{
public:
	std::vector<Vertex2D> vertices = {
		{ { 0.0f, -0.5f},{ 1.0f, 0.0f, 0.0f }},
		{ { 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f }},
		{ { -0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f }}		
	};

	void initialize(){
		//Create bufferss
		wxjCreateVertexBuffer<Vertex2D>(vertices);
		wxjCreateCommandBuffer();

		wxjCreateSwapChainImagesAndImageViews();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		wxjCreateFramebuffers();

		//Create shader resource
		shaderManager.InitVertexShader("../shaders/simpleVertexBuffer/vert.spv");
		shaderManager.InitFragmentShader("../shaders/simpleVertexBuffer/frag.spv");

		//Create Descriptors
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets();

		renderProcess.createGraphicsPipeline<Vertex2D>(
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
		wxjBeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
		wxjBeginRenderPass(clearValues);

		wxjBindPipeline();
		wxjSetViewport();
		wxjSetScissor();
		wxjBindVertexBuffer();
		wxjBindDescriptorSets();
		wxjDraw(3);

		wxjEndRenderPass();
		wxjEndCOmmandBuffer();

		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"


