#include "..\\framework\\include\\application.h"
#include <random>

#define TEST_CLASS_NAME CSimpleParticles
class TEST_CLASS_NAME: public CApplication{
public:
	static const uint32_t PARTICLE_COUNT = 8192;

	struct StructCustomUniformBuffer {
		float deltaTime = 1.0f;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			return binding;
		}
	};
	StructCustomUniformBuffer customUniformBufferObject{};

	struct Particle {
		glm::vec2 position;
		glm::vec2 velocity;
		glm::vec4 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Particle);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Particle, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Particle, color);

			return attributeDescriptions;
		}
	};


	struct StructStorageBufferInput {
		Particle particlesIn[PARTICLE_COUNT];
	};
	StructStorageBufferInput storageBufferObjectInput;
	struct StructStorageBufferOutput {
		Particle particlesOut[PARTICLE_COUNT];
	};
	StructStorageBufferOutput storageBufferObjectOutput;	

	bool bVerbose = true;
	bool bVerify = false;

	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	void initialize(){
		renderer.CreateCommandPool(surface);
		
		//For Graphics
		renderer.CreateGraphicsCommandBuffer(); 

		//For Compute
		renderer.CreateComputeCommandBuffer();

		//For Graphics
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); 
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();
		swapchain.CreateFramebuffers(renderProcess.renderPass);
		shaderManager.CreateShader("simpleParticles/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleParticles/frag.spv", shaderManager.FRAG); 

		//For Compute
		shaderManager.CreateShader("simpleParticles/comp.spv", shaderManager.COMP);
		std::cout<<"compute shader created."<<std::endl;

		descriptors.resize(2);//you can have many descriptor sets. Here I use ds[0] for graphics pipeline and ds[1] for compute pipeline
		//For Graphics
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets();

		std::cout<<"graphcis descriptor created."<<std::endl;

		//For Compute
		descriptors[1].addCustomUniformBuffer(sizeof(StructCustomUniformBuffer));
		descriptors[1].addStorageBuffer(sizeof(StructStorageBufferInput));
		//std::cout<<"storageBufferObjectInput added. Size = "<<DIM_M * DIM_K * 8.0f * 2 / 1024 / 1024<<"mb."<<std::endl;
		descriptors[1].addStorageBuffer(sizeof(StructStorageBufferOutput));
		//std::cout<<"storageBufferObjectOutput added. Size = "<<DIM_M * DIM_K * 8.0f / 1024 / 1024<<"mb."<<std::endl;
		std::cout<<"add storage buffer done."<<std::endl;
		descriptors[1].createDescriptorPool(); std::cout<<"createDescriptorPool done."<<std::endl;
		VkDescriptorSetLayoutBinding customBinding = StructCustomUniformBuffer::GetBinding();
		descriptors[1].createDescriptorSetLayout(&customBinding); std::cout<<"createDescriptorSetLayout done."<<std::endl;
		descriptors[1].createDescriptorSets(); std::cout<<"createDescriptorSets done."<<std::endl;

		std::cout<<"compute descriptor created."<<std::endl;

		//for Graphics: when create graphics pipeline, use descriptor set 0 layout
		renderProcess.createGraphicsPipelineLayout(descriptors[0].descriptorSetLayout);
		renderProcess.createGraphicsPipeline<Particle>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		std::cout<<"graphics pipeline created."<<std::endl;

		//For Compute: when create compute pipeline, use descriptor set 1 layout
		renderProcess.createComputePipelineLayout(descriptors[1].descriptorSetLayout);
		renderProcess.createComputePipeline(shaderManager.compShaderModule);

		std::cout<<"compute pipeline created."<<std::endl;

		CApplication::initialize();
		
		std::cout<<"init done."<<std::endl;

		//Initial Host data
		//Initialize particles
        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
        // Initial particle positions on a circle

		//Need put particles to storageBufferObjectInput
		//Can write a loop, assign the values to storageBufferObjectInput
		//Or, create staging buffer, and call memcpy

        //std::vector<Particle> particles(PARTICLE_COUNT);
        //for (auto& particle : particles) {
		for(int i = 0; i < PARTICLE_COUNT; i++){
            float r = 0.25f * sqrt(rndDist(rndEngine));
            float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
            float x = r * cos(theta) * windowHeight / windowWidth;
            float y = r * sin(theta);
			storageBufferObjectInput.particlesIn[i].position = glm::vec2(x, y);
            storageBufferObjectInput.particlesIn[i].velocity = glm::normalize(glm::vec2(x,y)) * 0.00025f;
            storageBufferObjectInput.particlesIn[i].color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
        }
		//VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;
		/*
		storageBufferObjectInput.M = DIM_M;
		storageBufferObjectInput.N = DIM_N;
		storageBufferObjectInput.K = DIM_K;
		for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObjectInput.MatA[i] = (float)rand() / (float)RAND_MAX;
		for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObjectInput.MatB[i] = (float)rand() / (float)RAND_MAX;
		if(bVerbose) PRINT("A: ", storageBufferObjectInput.MatA, DIM_M*DIM_K);
		if(bVerbose) PRINT("B: ", storageBufferObjectInput.MatB, DIM_K*DIM_N);
		if(bVerbose) PRINT("");
		std::cout<<"Initialized A and B."<<std::endl;
		*/

		//Host >> Device
		descriptors[1].updateStorageBuffer_1<StructStorageBufferInput>(renderer.currentFrame, durationTime, storageBufferObjectInput);
		descriptors[1].updateStorageBuffer_1<StructStorageBufferInput>(renderer.currentFrame+1, durationTime, storageBufferObjectInput);
		
		std::cout<<"Host >> Device done."<<std::endl;
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

		//VkBuffer vertexBuffers[] = {vertexDataBuffer.buffer };

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 0, 1, &descriptors[1].storageBuffers_1[renderer.currentFrame].buffer, offsets);

		renderer.Draw(PARTICLE_COUNT);
		
		END_GRAPHICS_RECORD
	}

	void recordComputeCommandBuffer(){
		START_COMPUTE_RECORD(1)


		renderer.Dispatch(PARTICLE_COUNT / 256, 1, 1);
		static int counter = 0;
		if(counter % 1000 == 0)std::cout<<"Dispatched Counter: "<<counter<<std::endl;
		counter++;

		END_COMPUTE_RECORD
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		//CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);

		//Device >> Host
		//if(bVerbose) memcpy(storageBufferObjectOutput.MatC, descriptors[1].storageBuffersMapped_2[renderer.currentFrame], sizeof(storageBufferObjectOutput.MatC));
		//if(bVerbose) printMatrix(storageBufferObjectOutput.MatC, DIM_M, DIM_N, "C");
		//if(bVerbose) PRINT("C: ", storageBufferObjectOutput.MatC, DIM_M*DIM_N);

	}
};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif

