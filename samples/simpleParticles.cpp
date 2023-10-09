#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleParticles
class TEST_CLASS_NAME: public CApplication{
public:
	static const int DIM = 16;
	static const int DIM_M = DIM;
	static const int DIM_K = DIM;
	static const int DIM_N = DIM;
	//static const int KernelRunNumber = 1;

	struct StructStorageBufferInput {
		unsigned int M;
		unsigned int K;
		unsigned int N;
		float MatA[DIM_M * DIM_K];
		float MatB[DIM_K * DIM_N];
	};
	StructStorageBufferInput storageBufferObjectInput;
	struct StructStorageBufferOutput {
		float MatC[DIM_M * DIM_N];
	};
	StructStorageBufferOutput storageBufferObjectOutput;	

	bool bVerbose = true;
	bool bVerify = false;

	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	void initialize(){
		renderer.CreateCommandPool(surface);
		renderer.CreateGraphicsCommandBuffer(); 
		renderer.CreateComputeCommandBuffer();

		//For Graphics
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); 
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		//For Graphics
		swapchain.CreateFramebuffers(renderProcess.renderPass);

		//For Graphics
		shaderManager.CreateShader("simpleTriangle/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleTriangle/frag.spv", shaderManager.FRAG); 

		//For Compute
		shaderManager.CreateShader("gemmCompute/comp.spv", shaderManager.COMP);
		std::cout<<"compute shader created."<<std::endl;

		descriptors.resize(2);
		//For Graphics
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets();
		//For Compute
		descriptors[1].addStorageBuffer(sizeof(StructStorageBufferInput));
		std::cout<<"storageBufferObjectInput added. Size = "<<DIM_M * DIM_K * 8.0f * 2 / 1024 / 1024<<"mb."<<std::endl;
		descriptors[1].addStorageBuffer(sizeof(StructStorageBufferOutput));
		std::cout<<"storageBufferObjectOutput added. Size = "<<DIM_M * DIM_K * 8.0f / 1024 / 1024<<"mb."<<std::endl;
		descriptors[1].createDescriptorPool();
		descriptors[1].createDescriptorSetLayout();
		descriptors[1].createDescriptorSets();

		//for Graphics
		//!compute and graphics pipeline should have different pipeline layout(already did), and different descriptorSetLayout
		//?different command buffers?
		renderProcess.createLayout(descriptors[0].descriptorSetLayout);//!this only create graphics pipline layout, should make it support compute pipeline layout
		renderProcess.createGraphicsPipeline(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		//For Compute
		renderProcess.createComputePipeline(shaderManager.compShaderModule, descriptors[1].descriptorSetLayout);

		CApplication::initialize();
		
		//Initial Host data
		storageBufferObjectInput.M = DIM_M;
		storageBufferObjectInput.N = DIM_N;
		storageBufferObjectInput.K = DIM_K;
		for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObjectInput.MatA[i] = (float)rand() / (float)RAND_MAX;
		for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObjectInput.MatB[i] = (float)rand() / (float)RAND_MAX;
		//if(bVerbose) printMatrix(storageBufferObjectInput.MatA, DIM_M, DIM_K, "A");
		//if(bVerbose) printMatrix(storageBufferObjectInput.MatB, DIM_K, DIM_N, "B");
		if(bVerbose) PRINT("A: ", storageBufferObjectInput.MatA, DIM_M*DIM_K);
		if(bVerbose) PRINT("B: ", storageBufferObjectInput.MatB, DIM_K*DIM_N);
		if(bVerbose) PRINT("");
		std::cout<<"Initialized A and B."<<std::endl;

		//Host >> Device
		descriptors[1].updateStorageBuffer_1<StructStorageBufferInput>(renderer.currentFrame, durationTime, storageBufferObjectInput);
		descriptors[1].updateStorageBuffer_1<StructStorageBufferInput>(renderer.currentFrame+1, durationTime, storageBufferObjectInput);
		
	}

	void update(){
		//static int counter = 1;
		//if(counter==KernelRunNumber) NeedToExit = true;
		//counter++;
		
		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)
		//actually this sample doesn't need BindDescriptorSets

		renderer.Draw(3);
		
		END_GRAPHICS_RECORD
	}

	void recordComputeCommandBuffer(){
		START_COMPUTE_RECORD(1)


		renderer.Dispatch(1, 1, 1);
		static int counter = 0;
		std::cout<<"Dispatched Counter: "<<counter<<std::endl;
		counter++;

		END_COMPUTE_RECORD
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		//CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);

		//Device >> Host
		if(bVerbose) memcpy(storageBufferObjectOutput.MatC, descriptors[1].storageBuffersMapped_2[renderer.currentFrame], sizeof(storageBufferObjectOutput.MatC));
		//if(bVerbose) printMatrix(storageBufferObjectOutput.MatC, DIM_M, DIM_N, "C");
		if(bVerbose) PRINT("C: ", storageBufferObjectOutput.MatC, DIM_M*DIM_N);

	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

