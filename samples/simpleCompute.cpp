#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleCompute
class TEST_CLASS_NAME: public CApplication{
public:
	const int KernelRunNumber = 5;
	struct StructStorageBuffer {
		glm::vec4 data;
	};
	StructStorageBuffer storageBufferObject;

	//std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

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
		renderer.CreateComputeCommandBuffer();

		//renderProcess.addColorAttachment(swapchain.swapChainImageFormat); //add this function will enable color attachment (bUseColorAttachment = true)
		//renderProcess.createSubpass();
		//renderProcess.createDependency();
		//renderProcess.createRenderPass();

		//swapchain.CreateFramebuffers(renderProcess.renderPass);

		//shaderManager.InitVertexShader("../shaders/simpleTriangle/vert.spv");
		//shaderManager.InitFragmentShader("../shaders/simpleTriangle/frag.spv");
		shaderManager.CreateShader("simpleCompute/comp.spv", shaderManager.COMP);

		descriptors[0].addStorageBuffer(sizeof(StructStorageBuffer));
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets();

		// renderProcess.createLayout(descriptor.descriptorSetLayout);
		// renderProcess.createGraphicsPipeline(
		// 	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
		// 	shaderManager.vertShaderModule, 
		// 	shaderManager.fragShaderModule);

		renderProcess.createComputePipeline(shaderManager.compShaderModule, descriptors[0].descriptorSetLayout);

		CApplication::initialize();
	}

	void update(){
		static int counter = 1;
		//Initial Host data
		PRINT("update(): write counter = %d to the device, frame = %d", counter, renderer.currentFrame);
		storageBufferObject.data = {counter+0.0f, counter+0.1f, counter+0.2f, counter+0.3f};

		//Host >> Device
		descriptors[0].updateStorageBuffer_1<StructStorageBuffer>(renderer.currentFrame, durationTime, storageBufferObject);
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;

		if(counter==KernelRunNumber) NeedToExit = true;
		counter++;

		CApplication::update(); //update time
	}

	void recordComputeCommandBuffer(){
		START_COMPUTE_RECORD(0)

		//std::cout<<"Record Compute command buffer. "<<std::endl;
		renderer.Dispatch(1, 1, 1);

		END_COMPUTE_RECORD
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		//Device >> Host
		float data[4] = {0};
		//std::cout<<"compute(): Current Frame = "<<renderer.currentFrame<<": "<<std::endl;
		memcpy(data, descriptors[0].storageBuffersMapped_1[renderer.currentFrame], sizeof(data));

		PRINT("compute() read data: ", data, 4);
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

