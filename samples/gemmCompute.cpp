#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CGemmCompute
class TEST_CLASS_NAME: public CApplication{
public:
	static const int DIM = 3;
	static const int DIM_M = DIM;
	static const int DIM_K = DIM;
	static const int DIM_N = DIM;
	static const int KernelRunNumber = 1;

	struct StructStorageBuffer {
		unsigned int M;
		unsigned int K;
		unsigned int N;
		float MatA[DIM_M * DIM_K];
		float MatB[DIM_K * DIM_N];
		float MatC[DIM_M * DIM_N];
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
		shaderManager.CreateComputeShader("gemmCompute/comp.spv");

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
		static int counter = 1;

		//Initial Host data
		storageBufferObject.M = DIM_M;
		storageBufferObject.N = DIM_N;
		storageBufferObject.K = DIM_K;
		for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObject.MatA[i] = (float)rand() / (float)RAND_MAX;
		for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObject.MatB[i] = (float)rand() / (float)RAND_MAX;


		//Host >> Device
		//std::cout<<"update(): write counter = "<<counter<<" to the device at frame="<<renderer.currentFrame<<std::endl;
		//storageBufferObject.data = {counter+0.0f, counter+0.1f, counter+0.2f, counter+0.3f};
		descriptor.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, durationTime, storageBufferObject);
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		
    	//vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		if(counter==KernelRunNumber) NeedToExit = true;
		counter++;

		CApplication::update(); //update time
	}

	void recordComputeCommandBuffer(){
		//Set kernel prameters. Launch Kernel on device
		std::cout<<"Dispatch Kernel. "<<std::endl;
		renderer.drawComputeFrame(renderProcess.computePipeline, renderProcess.computePipelineLayout, descriptor.descriptorSets);

		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());


		//Device >> Host
		unsigned int data[DIM*DIM*3] = {0};
		//std::cout<<"compute(): Current Frame = "<<renderer.currentFrame<<": "<<std::endl;
		memcpy(data, descriptor.storageBuffersMapped[renderer.currentFrame], sizeof(data));
		std::cout<<"compute(): read data = {"<<std::endl;	
		for(int i = 0; i < DIM*DIM*3; i++) std::cout<<data[i]<<", ";
		std::cout<<"} from the device at frame="<<renderer.currentFrame<<std::endl;	
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

