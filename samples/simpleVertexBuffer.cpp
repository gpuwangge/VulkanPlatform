#include "..\\framework\\include\\application.h"
#include "object.h"
#define TEST_CLASS_NAME CSimpleVertexBuffer
class TEST_CLASS_NAME: public CApplication{
//a triangle on the screen, each vertex has R, G or B color. Fragment color is interpolated
public:
	std::vector<Vertex2D> vertices = {
		{ { 0.0f, -0.5f},{ 1.0f, 0.0f, 0.0f }},
		{ { 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f }},
		{ { -0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f }}		
	};

	CObject object;

	void initialize(){
		object.Init((CApplication*)this, vertices);
		//triangleObject.InitVertices2D(vertices);
		//renderer.CreateVertexBuffer<Vertex2D>(triangleObject.vertices2D);

		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		shaderManager.CreateShader("simpleVertexBuffer/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleVertexBuffer/frag.spv", shaderManager.FRAG); 

		CDescriptorManager::createDescriptorPool(); 
		CGraphicsDescriptorManager::createDescriptorSetLayout(); 
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout();
		graphicsDescriptorManager.createDescriptorSets();

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);

		renderProcess.createGraphicsPipelineLayout(dsLayouts);
		renderProcess.createGraphicsPipeline<Vertex2D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer(){
		renderer.BindVertexBuffer(0);
		renderer.Draw(3);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

