#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CGemmCompute
class TEST_CLASS_NAME: public CApplication{
public:
	static const int DIM = 16;
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
	// struct StructStorageBufferOutput {
	// 	float MatC[DIM_M * DIM_N];
	// };
	// StructStorageBufferOutput storageBufferObjectOutput;	

	bool bVerbose = true;
	bool bVerify = true;

	void initialize(){
		renderer.m_renderMode = renderer.RENDER_COMPUTE_Mode;
		renderer.CreateCommandPool(surface);
		renderer.CreateComputeCommandBuffer();

		shaderManager.CreateShader("gemmCompute/comp.spv", shaderManager.COMP);
		std::cout<<"compute shader created."<<std::endl;

		descriptors[0].addStorageBuffer(sizeof(StructStorageBuffer));
		//std::cout<<"storageBufferObjectInput added. Size = "<<DIM_M * DIM_K * 8.0f * 3 / 1024 / 1024<<"mb."<<std::endl;
		//descriptors[0].addStorageBuffer(sizeof(StructStorageBufferOutput));
		//std::cout<<"storageBufferObjectOutput added. Size = "<<DIM_M * DIM_K * 8.0f / 1024 / 1024<<"mb."<<std::endl;
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets();

		renderProcess.createComputePipelineLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createComputePipeline(shaderManager.compShaderModule);

		CApplication::initialize();

		
		//Initial Host data
		storageBufferObject.M = DIM_M;
		storageBufferObject.N = DIM_N;
		storageBufferObject.K = DIM_K;
		for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObject.MatA[i] = (float)rand() / (float)RAND_MAX;
		for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObject.MatB[i] = (float)rand() / (float)RAND_MAX;
		//if(bVerbose) printMatrix(storageBufferObjectInput.MatA, DIM_M, DIM_K, "A");
		//if(bVerbose) printMatrix(storageBufferObjectInput.MatB, DIM_K, DIM_N, "B");
		if(bVerbose) PRINT("A: ", storageBufferObject.MatA, DIM_M*DIM_K);
		if(bVerbose) PRINT("B: ", storageBufferObject.MatB, DIM_K*DIM_N);
		if(bVerbose) PRINT("");
		std::cout<<"Initialized A and B."<<std::endl;

		//Host >> Device
		descriptors[0].updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, durationTime, storageBufferObject); //1 TODO: fill all inflight, update input only
		descriptors[0].updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame+1, durationTime, storageBufferObject); //1
		
	}

	void update(){
		std::cout<<"update."<<std::endl;
		static int counter = 1;
		if(counter==KernelRunNumber) NeedToExit = true;
		counter++;
		
		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
	}

	void recordComputeCommandBuffer(){
		START_COMPUTE_RECORD(0)

		//std::cout<<"Record Compute command buffer. "<<std::endl;
		renderer.Dispatch(1, 1, 1);
		std::cout<<"Dispatched."<<std::endl;

		END_COMPUTE_RECORD
	}

	void postUpdate(){
		std::cout<<"postUpdate."<<std::endl;
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);

		//Device >> Host
		//if(bVerbose) memcpy(storageBufferObject.MatC, descriptors[0].storageBuffersMapped[renderer.currentFrame], sizeof(storageBufferObject.MatC));//2
		if(bVerbose) memcpy(&storageBufferObject, descriptors[0].storageBuffersMapped[renderer.currentFrame], sizeof(storageBufferObject));//2 TODO: how to copy only MatC
		//if(bVerbose) printMatrix(storageBufferObjectOutput.MatC, DIM_M, DIM_N, "C");
		if(bVerbose) PRINT("C: ", storageBufferObject.MatC, DIM_M*DIM_N);

		if(bVerify){
			//std::cout<<"Begin verification..."<<std::endl;
			PRINT("Begin verification... ");
			float cpu_result[DIM_M*DIM_N];
			CPUSingleThreadMatMul(DIM_M, DIM_N, DIM_K, storageBufferObject.MatA, storageBufferObject.MatB, cpu_result, DIM_M*DIM_N);
			//printMatrix(cpu_result, DIM_M, DIM_N, "cpu_C");
			PRINT("cpu_C: ", cpu_result, DIM_M*DIM_N);
		}

		if(bVerbose) PRINT("");
	}

/*
	void printMatrix(float *matA, int DIM_M, int DIM_N, std::string matName){
#ifndef ANDROID		
		std::cout<<"Print "<<matName<<std::endl;
		for(int i=0; i < DIM_M; i++){
			for(int j=0; j < DIM_N; j++){
				std::cout<<matA[i*DIM_N + j]<<", ";
			}
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
#else
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Print %s\n", matName.c_str());
		for(int i=0; i < DIM_M; i++){
			for(int j=0; j < DIM_N; j++){
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "%f, ", matA[i*DIM_N + j]);
			}
			//__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "\n");
		}
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "\n");
#endif
	}
*/
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
					PRINT("Completed: %f%%", completeRate);
					//std::cout<<"Completed: "<<completeRate<<"%"<<std::endl;
				}
				
				if(count >= sampleNum) return;
			}
		}
	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

