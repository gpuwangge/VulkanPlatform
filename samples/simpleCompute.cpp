#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleCompute
class TEST_CLASS_NAME: public CApplication{
public:
	struct StructStorageBuffer {
		glm::vec4 data;
	};
	StructStorageBuffer storageBufferObject;

	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	void initialize(){
		//0. prepare compute shader(spv)*
		//1. need a physical device with compute, and pick Queue Family with compute*
		//2. create compute command buffer*
		//3. create storage buffer* (initial data with fill())
		//4. create layouts for compute shader*
		//5. create compute pipeline (with shader and layout information)*
		//6. record command (bind compute shader and dispatch)*
		//Question: If I edit storage buffer in shader, will it reflect?
		
		renderer.CreateCommandPool(surface);
		//renderer.CreateCommandBuffers();
		renderer.CreateComputeCommandBuffers();

		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.InitVertexShader("../shaders/simpleTriangle/vert.spv");
		//shaderManager.InitFragmentShader("../shaders/simpleTriangle/frag.spv");
		shaderManager.CreateComputeShader("simpleCompute/comp.spv");


		descriptor.addStorageBuffer(sizeof(StructStorageBuffer));
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(textureImages);

		// renderProcess.createLayout(descriptor.descriptorSetLayout);
		// renderProcess.createGraphicsPipeline(
		// 	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
		// 	shaderManager.vertShaderModule, 
		// 	shaderManager.fragShaderModule);
		renderProcess.createComputePipeline(shaderManager.computeShaderModule, descriptor.descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		//Host >> Device
		storageBufferObject.data = {1.2, 2.3, 3.4, 4.5};
		descriptor.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, durationTime, storageBufferObject);
		CApplication::update();
	}

	void recordCommandBuffer(){
		//RENDER_START
		//actually this sample doesn't need BindDescriptorSets

		//renderer.Draw(3);
		renderer.drawComputeFrame(renderProcess.pipeline_compute, renderProcess.pipelineLayout_compute, descriptor.descriptorSets);
		

		//Device >> Host
		float data[4] = {0};
		std::cout<<"Current Frame = "<<renderer.currentFrame<<": ";
		memcpy(data, descriptor.storageBuffersMapped[renderer.currentFrame], sizeof(data));
		std::cout<<data[0]<<", "<<data[1]<<", "<<data[2]<<", "<<data[3]<<std::endl;

		//RENDER_END
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

