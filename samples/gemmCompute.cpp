#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CGemmCompute
class TEST_CLASS_NAME: public CApplication{
public:
	static const int DIM = 32;
	static const int DIM_M = DIM;
	static const int DIM_K = DIM;
	static const int DIM_N = DIM;
	static const int KernelRunNumber = 5;

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

	bool bVerbose = false;
	bool bVerify = false;

	void initialize(){
		renderer.CreateCommandPool(surface);
		//renderer.CreateCommandBuffers();
		renderer.CreateComputeCommandBuffers();

		shaderManager.CreateComputeShader("gemmCompute/comp.spv");

		descriptor.addStorageBuffer(sizeof(StructStorageBufferInput));
		descriptor.addStorageBuffer(sizeof(StructStorageBufferOutput));
		descriptor.createDescriptorPool();
		descriptor.createDescriptorSetLayout();
		descriptor.createDescriptorSets(textureImages);

		renderProcess.createComputePipeline(shaderManager.computeShaderModule, descriptor.descriptorSetLayout);

		CApplication::initialize();

		
		//Initial Host data
		storageBufferObjectInput.M = DIM_M;
		storageBufferObjectInput.N = DIM_N;
		storageBufferObjectInput.K = DIM_K;
		for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObjectInput.MatA[i] = (float)rand() / (float)RAND_MAX;
		for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObjectInput.MatB[i] = (float)rand() / (float)RAND_MAX;
		if(bVerbose) printMatrix(storageBufferObjectInput.MatA, DIM_M, DIM_K, "A");
		if(bVerbose) printMatrix(storageBufferObjectInput.MatB, DIM_K, DIM_N, "B");

		//Host >> Device
		descriptor.updateStorageBuffer_1<StructStorageBufferInput>(renderer.currentFrame, durationTime, storageBufferObjectInput);
		descriptor.updateStorageBuffer_1<StructStorageBufferInput>(renderer.currentFrame+1, durationTime, storageBufferObjectInput);
		
	}

	void update(){
		static int counter = 1;

		if(counter==KernelRunNumber) NeedToExit = true;
		counter++;

		
		CApplication::update(); //update time
		std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
	}

	void recordComputeCommandBuffer(){
		//Set kernel prameters. Launch Kernel on device
		std::cout<<"Dispatch Kernel. "<<std::endl;
		//renderer.drawComputeFrame(renderProcess.computePipeline, renderProcess.computePipelineLayout, descriptor.descriptorSets);

///////////////
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(renderer.commandBuffers_compute[renderer.currentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording compute command buffer!");
		}

		vkCmdBindPipeline(renderer.commandBuffers_compute[renderer.currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, renderProcess.computePipeline);

		vkCmdBindDescriptorSets(renderer.commandBuffers_compute[renderer.currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, renderProcess.computePipelineLayout, 0, 1, &descriptor.descriptorSets[renderer.currentFrame], 0, nullptr);

		vkCmdDispatch(renderer.commandBuffers_compute[renderer.currentFrame], 1, 1, 1); //TODO: set workgroup number as parameter

		if (vkEndCommandBuffer(renderer.commandBuffers_compute[renderer.currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record compute command buffer!");
		}
///////////////////////


		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		//Device >> Host
		if(bVerbose) memcpy(storageBufferObjectOutput.MatC, descriptor.storageBuffersMapped_2[renderer.currentFrame], sizeof(storageBufferObjectOutput.MatC));
		if(bVerbose) printMatrix(storageBufferObjectOutput.MatC, DIM_M, DIM_N, "C");

		if(bVerify){
			float cpu_result[DIM_M*DIM_N];
			CPUSingleThreadMatMul(DIM_M, DIM_N, DIM_K, storageBufferObjectInput.MatA, storageBufferObjectInput.MatB, cpu_result, 9);
			printMatrix(cpu_result, DIM_M, DIM_N, "cpu_C");
		}
	}

	void printMatrix(float *matA, int DIM_M, int DIM_N, std::string matName){
		std::cout<<"Print "<<matName<<std::endl;
		for(int i=0; i < DIM_M; i++){
			for(int j=0; j < DIM_N; j++){
				std::cout<<matA[i*DIM_N + j]<<", ";
			}
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
	}

	void CPUSingleThreadMatMul(int M, int N, int K, float *matrixA, float *matrixB, float *outputMatrix, int sampleNum){
		int count = 0;
		int printDelta = sampleNum / 1;
		for(int n = 0; n < N; n++){ //col
			for(int m = 0; m < M; m++){ //row： fill row first, allign with kernel
				//column major matrix multiplication
				outputMatrix[n*M + m] = 0;
				for(int k = 0; k < K; k++){
					outputMatrix[n*M + m] += matrixA[k*M + m] * matrixB[n*K + k];
				}

				count++;
				if(count % printDelta == 0){
					float completeRate = (count * 100.0)/sampleNum ;
					std::cout<<"Completed: "<<completeRate<<"%"<<std::endl;
				}
				
				if(count >= sampleNum) return;
			}
		}
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

