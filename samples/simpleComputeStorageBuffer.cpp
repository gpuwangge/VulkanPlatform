#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleComputeStorageBuffer
class TEST_CLASS_NAME: public CApplication{
//A sample to test storage buffer (for compute)	
//This sample write 1, 2, 3, 4, 5 to storage buffer and read back
//storage buffer can be used for gemm
public:
	const int KernelRunNumber = 5;
	struct StructStorageBuffer {
		glm::vec4 data;
	};
	StructStorageBuffer storageBufferObject;

	void initialize(){
		renderer.m_renderMode = renderer.RENDER_COMPUTE_Mode;
		
		renderer.CreateCommandPool(surface);
		renderer.CreateComputeCommandBuffer();

		shaderManager.CreateShader("simpleComputeStorageBuffer/comp.spv", shaderManager.COMP);

		CComputeDescriptorManager::addStorageBuffer(sizeof(StructStorageBuffer));
		CDescriptorManager::createDescriptorPool();
		CComputeDescriptorManager::createDescriptorSetLayout();
		computeDescriptorManager.createDescriptorSets();

		renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
		renderProcess.createComputePipeline(shaderManager.compShaderModule);

		CApplication::initialize();
	}

	void update(){
		static int counter = 1;
		//Initial Host data
		PRINT("update(): write counter = %d to the device, frame = %d", counter, renderer.currentFrame);
		storageBufferObject.data = {counter+0.0f, counter+0.1f, counter+0.2f, counter+0.3f};

		//Host >> Device
		computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, durationTime, storageBufferObject);
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;

		if(counter==KernelRunNumber) NeedToExit = true; 
		counter++;

		CApplication::update(); //update time
	}

	void recordComputeCommandBuffer(){
		//std::cout<<"Record Compute command buffer. "<<std::endl;
		renderer.Dispatch(1, 1, 1); //dispatch an empty compute shader. TODO: need add some compute here
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		//Device >> Host
		float data[4] = {0};
		//std::cout<<"compute(): Current Frame = "<<renderer.currentFrame<<": "<<std::endl;
		memcpy(data, computeDescriptorManager.storageBuffersMapped[renderer.currentFrame], sizeof(data));

		PRINT("compute() read data: ", data, 4);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

