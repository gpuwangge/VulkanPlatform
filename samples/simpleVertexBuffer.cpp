#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleVertexBuffer
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex2D> vertices = {
		{ { 0.0f, -0.5f},{ 1.0f, 0.0f, 0.0f }},
		{ { 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f }},
		{ { -0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f }}		
	};

	void initialize(){
		//Create bufferss
		renderer.CreateVertexBuffer<Vertex2D>(vertices);
		renderer.InitCreateCommandPool(surface);
		renderer.InitCreateCommandBuffers();

		//Create Renderpass
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		//wxjCreateFramebuffers();
		swapchain.CreateFramebuffers(renderProcess.bUseDepthAttachment, renderProcess.bUseColorAttachmentResolve, renderProcess.renderPass);

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
		renderer.BeginCommandBuffer();

		std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
		renderer.BeginRenderPass(renderProcess.renderPass, swapchain.swapChainFramebuffers, swapchain.swapChainExtent, clearValues);

		renderer.BindPipeline(renderProcess.graphicsPipeline);
		renderer.SetViewport(swapchain.swapChainExtent);
		renderer.SetScissor(swapchain.swapChainExtent);
		renderer.BindVertexBuffer();
		renderer.BindDescriptorSets(renderProcess.pipelineLayout, descriptor.descriptorSets);
		renderer.Draw(3);

		renderer.EndRenderPass();
		renderer.EndCOmmandBuffer();

		CApplication::recordCommandBuffer();
	}
};

#include "..\\framework\\include\\main.hpp"


