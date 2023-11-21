#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CTextureCompute
class TEST_CLASS_NAME: public CApplication{
public:
	//Compute Shader Constants
	// static const int DIM = 16;
	// static const int DIM_M = DIM;
	// static const int DIM_K = DIM;
	// static const int DIM_N = DIM;
	static const int KernelRunNumber = 1;
	// struct StructStorageBuffer {
	// 	unsigned int M;
	// 	unsigned int K;
	// 	unsigned int N;
	// 	float MatA[DIM_M * DIM_K];
	// 	float MatB[DIM_K * DIM_N];
	// 	float MatC[DIM_M * DIM_N];
	// };
	// StructStorageBuffer storageBufferObject;
	// struct StructStorageBufferOutput {
	// 	float MatC[DIM_M * DIM_N];
	// };
	// StructStorageBufferOutput storageBufferObjectOutput;	
	bool bVerbose = true;
	bool bVerify = true;

	//Graphics Constants
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	CTextureCompute(){
		swapchain.imageSize = MAX_FRAMES_IN_FLIGHT;
	}

	void initialize(){
		//For Grapics
		renderer.CreateVertexBuffer<Vertex3D>(vertices3D);
		renderer.CreateIndexBuffer(indices3D);

		renderer.CreateCommandPool(surface);
		
		//For Graphics
		renderer.CreateGraphicsCommandBuffer(); 
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		textureImages[0].CreateTextureImage("texture.jpg", usage, renderer.commandPool);
		textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

		//For Compute
		renderer.CreateComputeCommandBuffer();

		//For Graphics
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); 
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();
		//renderProcess.addColorBlendAttachment(
		//	VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		//	VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);
		swapchain.CreateFramebuffers(renderProcess.renderPass);
		shaderManager.CreateShader("textureCompute/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("textureCompute/frag.spv", shaderManager.FRAG); 

		//For Compute
		shaderManager.CreateShader("textureCompute/comp.spv", shaderManager.COMP);
		std::cout<<"compute shader created."<<std::endl;


		descriptors.resize(2);//you can have many descriptor sets. Here I use ds[0] for graphics pipeline and ds[1] for compute pipeline
		//For Graphics
		descriptors[0].addMVPUniformBuffer();
		descriptors[0].addImageSamplerUniformBuffer(textureImages[0].mipLevels);
		descriptors[0].createDescriptorPool();
		descriptors[0].createDescriptorSetLayout();
		descriptors[0].createDescriptorSets(&textureImages);
		std::cout<<"graphcis descriptor created."<<std::endl;

		//For Compute
		//descriptors[1].addStorageBuffer(sizeof(StructStorageBuffer));
		//std::cout<<"storageBufferObjectInput added. Size = "<<DIM_M * DIM_K * 8.0f * 3 / 1024 / 1024<<"mb."<<std::endl;
		//descriptors[1].addStorageBuffer(sizeof(StructStorageBufferOutput));
		//std::cout<<"storageBufferObjectOutput added. Size = "<<DIM_M * DIM_K * 8.0f / 1024 / 1024<<"mb."<<std::endl;
		//descriptors[1].addStorageImage(textureImages[0].textureImageBuffer.size);//assume 4 channels and 4 bytes/channel
		//std::cout<<"storageImage added. Size = "<<textureImages[0].texHeight * textureImages[0].texWidth * 4 * 4<<" bytes."<<std::endl;
		//descriptors[1].addStorageImage(sizeof(StructStorageBuffer));
		//std::cout<<"addStorageImage() done."<<std::endl;
		descriptors[1].uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;
		descriptors[1].uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;
		descriptors[1].createDescriptorPool();
		std::cout<<"createDescriptorPool() done."<<std::endl;
		descriptors[1].createDescriptorSetLayout();
		std::cout<<"createDescriptorSetLayout() done."<<std::endl;
		//descriptors[1].createDescriptorSets(&textureImages);
		descriptors[1].createDescriptorSets(&textureImages, &(swapchain.views));
		std::cout<<"compute descriptor created."<<std::endl;

		//for Graphics: when create graphics pipeline, use descriptor set 0 layout
		renderProcess.createGraphicsPipelineLayout(descriptors[0].descriptorSetLayout);
		std::cout<<"createGraphicsPipelineLayout() done."<<std::endl;
		renderProcess.createGraphicsPipeline<Vertex3D>(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			shaderManager.vertShaderModule, 
			shaderManager.fragShaderModule);
		std::cout<<"graphics pipeline created."<<std::endl;

		//For Compute: when create compute pipeline, use descriptor set 1 layout
		renderProcess.createComputePipelineLayout(descriptors[1].descriptorSetLayout);
		renderProcess.createComputePipeline(shaderManager.compShaderModule);

		CApplication::initialize();

		createComputeCommandBuffers(renderer.commandBuffers[renderer.computeCmdId], swapchain.images);

		//Initial Host data
		// storageBufferObject.M = DIM_M;
		// storageBufferObject.N = DIM_N;
		// storageBufferObject.K = DIM_K;
		// for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObject.MatA[i] = (float)rand() / (float)RAND_MAX;
		// for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObject.MatB[i] = (float)rand() / (float)RAND_MAX;
		// //if(bVerbose) printMatrix(storageBufferObjectInput.MatA, DIM_M, DIM_K, "A");
		// //if(bVerbose) printMatrix(storageBufferObjectInput.MatB, DIM_K, DIM_N, "B");
		// if(bVerbose) PRINT("A: ", storageBufferObject.MatA, DIM_M*DIM_K);
		// if(bVerbose) PRINT("B: ", storageBufferObject.MatB, DIM_K*DIM_N);
		// if(bVerbose) PRINT("");
		// std::cout<<"Initialized A and B."<<std::endl;

		//Host >> Device
		//descriptors[1].updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, durationTime, storageBufferObject); //1 TODO: fill all inflight, update input only
		//descriptors[1].updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame+1, durationTime, storageBufferObject); //1
		
	}

	void update(){
		static int counter = 1;
		//if(counter==KernelRunNumber) NeedToExit = true;
		counter++;
		
		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
	}

	void recordGraphicsCommandBuffer(){
		START_GRAPHICS_RECORD(0)
		//actually this sample doesn't need BindDescriptorSets

		//VkBuffer vertexBuffers[] = {vertexDataBuffer.buffer };

		//renderer.Draw(3);

		//renderer.BindVertexBuffer();
		//renderer.BindIndexBuffer();
		//renderer.DrawIndexed(indices3D);
		
		END_GRAPHICS_RECORD
	}

	void recordComputeCommandBuffer(){
		//START_COMPUTE_RECORD(1)

		//std::cout<<"Record Compute command buffer. "<<std::endl;
		//renderer.Dispatch(100, 1, 1);
		//std::cout<<"Dispatched."<<std::endl;

		//END_COMPUTE_RECORD
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);

		//Device >> Host
		//if(bVerbose) memcpy(storageBufferObject.MatC, descriptors[0].storageBuffersMapped[renderer.currentFrame], sizeof(storageBufferObject.MatC));//2
		//if(bVerbose) memcpy(&storageBufferObject, descriptors[1].storageBuffersMapped[renderer.currentFrame], sizeof(storageBufferObject));//2 TODO: how to copy only MatC
		//if(bVerbose) printMatrix(storageBufferObjectOutput.MatC, DIM_M, DIM_N, "C");
		//if(bVerbose) PRINT("C: ", storageBufferObject.MatC, DIM_M*DIM_N);


	    //std::cout<<textureImages[0].textureImageBuffer.size<<std::endl;
		//memcpy(textureImages[0].textureImageBuffer.deviceMemory, descriptors[1].storageImagesMapped[renderer.currentFrame], textureImages[0].textureImageBuffer.size);
		
		//float data[textureImages[0].textureImageBuffer.size];
		//memcpy(data, descriptors[1].storageImagesMapped[renderer.currentFrame], textureImages[0].textureImageBuffer.size);
		//for(int i = 0; i < textureImages[0].textureImageBuffer.size; i++)
			//PRINT("%f",data[i]);


		//std::cout<<"done"<<std::endl;

		// if(bVerify){
		// 	//std::cout<<"Begin verification..."<<std::endl;
		// 	PRINT("Begin verification... ");
		// 	float cpu_result[DIM_M*DIM_N];
		// 	CPUSingleThreadMatMul(DIM_M, DIM_N, DIM_K, storageBufferObject.MatA, storageBufferObject.MatB, cpu_result, DIM_M*DIM_N);
		// 	//printMatrix(cpu_result, DIM_M, DIM_N, "cpu_C");
		// 	PRINT("cpu_C: ", cpu_result, DIM_M*DIM_N);
		// }

		//if(bVerbose) PRINT("");
	}

	void recordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
        VkAccessFlags scrAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcBind, VkPipelineStageFlags dstBind) {
        VkImageMemoryBarrier barrier{};
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcAccessMask = scrAccess;
        barrier.dstAccessMask = dstAccess;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        VkImageSubresourceRange sub{};
        sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        sub.baseArrayLayer = 0;
        sub.baseMipLevel = 0;
        sub.layerCount = VK_REMAINING_MIP_LEVELS;
        sub.levelCount = VK_REMAINING_MIP_LEVELS;
        barrier.subresourceRange = sub;

        vkCmdPipelineBarrier(buffer, srcBind, dstBind,
            0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

	void createComputeCommandBuffers(std::vector<VkCommandBuffer> &commandBuffers, std::vector<VkImage> &swapChainImages) {
        for (size_t i = 0; i < commandBuffers.size(); i++) {
			renderer.currentFrame = i;
			std::cout<<"commandbuffer i: "<<i<<std::endl;
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            //if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            //    throw std::runtime_error("failed to begin recording command buffer!");
            //}
			renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout, descriptors[1].descriptorSets);

            recordImageBarrier(commandBuffers[i], swapChainImages[i],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            
			
            //vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
            //vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
            //vkCmdDispatch(commandBuffers[i], 200, 300, 1);
			renderer.Dispatch(300, 600, 1); 
         
            recordImageBarrier(commandBuffers[i], swapChainImages[i],
                VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

            //if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            //    throw std::runtime_error("failed to record command buffer!");
            //}
			renderer.EndRecordComputeCommandBuffer();
        }
		renderer.currentFrame = 0;
    }

};

#ifndef ANDROID
#include "..\\windowsFramework\\include\\main.hpp"
#endif
