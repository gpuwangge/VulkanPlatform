#include "..\\framework\\include\\application.h"
#include <random>
#include "object.h"
#define TEST_CLASS_NAME CSimpleParticles
class TEST_CLASS_NAME: public CApplication{
public:
	static const uint32_t PARTICLE_COUNT = 4096;//8192 will fail on Pixel 7 Pro

	struct StructCustomUniformBuffer {
		float deltaTime = 1.0f;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
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

	struct StructStorageBuffer {
		Particle particles[PARTICLE_COUNT];
	};
	StructStorageBuffer storageBufferObject;
	// struct StructStorageBufferOutput {
	// 	Particle particlesOut[PARTICLE_COUNT];
	// };
	//StructStorageBufferOutput storageBufferObjectOutput;	

	bool bVerbose = true;
	bool bVerify = false;

	std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };
	
	CObject object;

	void initialize(){
		renderer.m_renderMode = renderer.RENDER_COMPUTE_GRAPHICS_Mode;

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
		renderProcess.addColorBlendAttachment(
			VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);
		swapchain.CreateFramebuffers(renderProcess.renderPass);
		shaderManager.CreateShader("simpleParticles/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("simpleParticles/frag.spv", shaderManager.FRAG); 

		//For Compute
		shaderManager.CreateShader("simpleParticles/comp.spv", shaderManager.COMP);
		std::cout<<"compute shader created."<<std::endl;


		//need make change for custom uniform: both graphics/compute should support custom uniform, their' layer/set code should contain source code
		//or, move the source code to parent manager?... (better not do this, because cant)
		CComputeDescriptorManager::addCustomUniformBuffer(sizeof(StructCustomUniformBuffer));
		VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		CComputeDescriptorManager::addStorageBuffer(sizeof(StructStorageBuffer), usage);

		CDescriptorManager::createDescriptorPool(); //size = 3(custom:1, storage:2)

		CGraphicsDescriptorManager::createDescriptorSetLayout();//!should not count custom here, because thats for compute
		VkDescriptorSetLayoutBinding customBinding = StructCustomUniformBuffer::GetBinding();
		CComputeDescriptorManager::createDescriptorSetLayout(&customBinding);

		graphicsDescriptorManager.createDescriptorSets();
		computeDescriptorManager.createDescriptorSets();

		object.Register((CApplication*)this);

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		std::vector<VkDescriptorSetLayout> dsLayouts;
		dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);

		//for Graphics: when create graphics pipeline, use descriptor set 0 layout
		renderProcess.createGraphicsPipelineLayout(dsLayouts);
		renderProcess.createGraphicsPipeline<Particle>(
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);

		std::cout<<"graphics pipeline created."<<std::endl;

		//For Compute: when create compute pipeline, use descriptor set 1 layout
		renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
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
			storageBufferObject.particles[i].position = glm::vec2(x, y);
            storageBufferObject.particles[i].velocity = glm::normalize(glm::vec2(x,y)) * 0.001f;
            storageBufferObject.particles[i].color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
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
		// Copy initial particle data to all storage buffers
		// Alternative: create particle info in a buffer, and copybuffer to all storage buffers
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
			computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(i, durationTime, storageBufferObject);
		//descriptors[1].updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame+1, durationTime, storageBufferObject);
		
		std::cout<<"Host >> Device done."<<std::endl;
	}

	void update(){
		//static int counter = 1;
		//if(counter==KernelRunNumber) NeedToExit = true;
		//counter++;
		customUniformBufferObject.deltaTime = deltaTime * 1000;
		//std::cout<<deltaTime<<std::endl;
		//customUniformBufferObject.color = {(sin(durationTime*3) + 1.0f) / 2.0f, (cos(durationTime*3) + 1.0f) / 2.0f, 0.0f, 1.0f};
		computeDescriptorManager.updateCustomUniformBuffer<StructCustomUniformBuffer>(renderer.currentFrame, durationTime, customUniformBufferObject);

		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
	}

	void recordGraphicsCommandBuffer(){
		//this sample doesn't need BindDescriptorSets

		//VkBuffer vertexBuffers[] = {vertexDataBuffer.buffer };

		//VkDeviceSize offsets[] = { 0 };
		//vkCmdBindVertexBuffers(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 0, 1, &computeDescriptorManager.storageBuffers[renderer.currentFrame].buffer, offsets);

		//renderer.Draw(PARTICLE_COUNT);
		object.Draw(computeDescriptorManager.storageBuffers, PARTICLE_COUNT);
	}

	void recordComputeCommandBuffer(){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		dsSets.push_back(computeDescriptorManager.descriptorSets);

		renderer.BindComputeDescriptorSets(renderProcess.computePipelineLayout, dsSets, -1); //-1 to offset means no dynamic offset

		renderer.Dispatch(PARTICLE_COUNT / 256, 1, 1);
		static int counter = 0;
		if(counter % 1000 == 0)std::cout<<"Dispatched Counter: "<<counter<<std::endl;
		counter++;
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

