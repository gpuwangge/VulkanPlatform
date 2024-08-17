#include "..\\framework\\include\\application.h"
#include "object.h"
#include "supervisor.h"
#define TEST_CLASS_NAME CTextureCompute
class TEST_CLASS_NAME: public CApplication{
//This test is similar to simpleComputeStorageImage, but instead use 2 texture image, one as input, the other as output
//It also render the result to swapchain
//simpleComputeStorageImage: write result to texture image and show to swapchain
//textureCompute: read texture image, make changes(use compute shader to do blur effect), and write to texture image, and show to swapchain
//This test use graphics pipeline but does not draw anything with it, because result is drawn directly to swap chain
//The reason for graphics pipeline is to load texture to texture image
//but why mvp? why color attachment?
//no need bind graphics pipeline?
//TODO: improve this sample, there is still something unknown for this test; something can be simplified
//A texture image is presented on the screen. Image is blured. char is blue/white. background is yellow.
public:
	static const int KernelRunNumber = 100;

	std::vector<std::pair<std::string, bool>> textureNames = {{"texture.jpg", false}}; //first: textureName, second: mipmap
	std::string computeShader = "textureCompute/comp.spv";

	TEST_CLASS_NAME(){//these must be updated before initialization
		swapchain.imageSize = MAX_FRAMES_IN_FLIGHT;
		swapchain.bComputeSwapChainImage = true;
	}

	void initialize(){
		renderer.m_renderMode = renderer.RENDER_COMPUTE_SWAPCHAIN_Mode;

		CSupervisor::Register((CApplication*)this);
		CSupervisor::ComputeShader = computeShader;
		CSupervisor::Activate_Uniform_Compute_StorageImage_Swapchain();
		CSupervisor::Activate_Uniform_Compute_StorageImage();
		CSupervisor::Activate_Texture(&textureNames);
		CSupervisor::Activate_Pipeline();

		/*
		renderer.CreateCommandPool(surface);
		
		//For Graphics
		renderer.CreateGraphicsCommandBuffer(); 

		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		textureManager.CreateTextureImage("texture.jpg", usage, renderer.commandPool, false, 
			swapchain.swapChainImageFormat);

		//For Compute
		
		renderer.CreateComputeCommandBuffer();

		//For Graphics
		renderProcess.addColorAttachment(swapchain.swapChainImageFormat); 
		renderProcess.createSubpass();
		renderProcess.createDependency();
		renderProcess.createRenderPass();

		swapchain.CreateFramebuffers(renderProcess.renderPass);
		shaderManager.CreateShader("textureCompute/vert.spv", shaderManager.VERT);
		shaderManager.CreateShader("textureCompute/frag.spv", shaderManager.FRAG); 

		//For Compute
		shaderManager.CreateShader("textureCompute/comp.spv", shaderManager.COMP);
		std::cout<<"compute shader created."<<std::endl;


		CGraphicsDescriptorManager::addMVPUniformBuffer();
		uint32_t mipLevels = textureManager.textureImages[0].mipLevels;
		CGraphicsDescriptorManager::addImageSamplerUniformBuffer(mipLevels);
		CComputeDescriptorManager::addStorageImage(UNIFORM_IMAGE_STORAGE_TEXTURE_BIT); //as input
		CComputeDescriptorManager::addStorageImage(UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT); //as output

		CDescriptorManager::createDescriptorPool(); //size = 4(mvp:1, sampler:1, storage image:2)

		CGraphicsDescriptorManager::createDescriptorSetLayout();
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout();
		CComputeDescriptorManager::createDescriptorSetLayout();
*/
		//graphicsDescriptorManager.createDescriptorSets(&textureManager.textureImages);
		

		//object.Register((CApplication*)this);//, vertices3D, indices3D
		//computeDescriptorManager.createDescriptorSets(&textureManager.textureImages, &(swapchain.views));

		//support multiple descriptors in one piplines: bind multiple descriptor layouts in one pipeline
		// std::vector<VkDescriptorSetLayout> dsLayouts;
		// dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
		// dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); 

		// //for Graphics: when create graphics pipeline, use descriptor set 0 layout
		// renderProcess.createGraphicsPipelineLayout(dsLayouts);
		// std::cout<<"createGraphicsPipelineLayout() done."<<std::endl;
		// renderProcess.createGraphicsPipeline<Vertex3D>(
		// 	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
		// 	shaderManager.vertShaderModule, 
		// 	shaderManager.fragShaderModule);
		// std::cout<<"graphics pipeline created."<<std::endl;

		//For Compute: when create compute pipeline, use descriptor set 1 layout
		//renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
		//renderProcess.createComputePipeline(shaderManager.compShaderModule);

		CApplication::initialize();

		createComputeCommandBuffers(renderer.commandBuffers[renderer.computeCmdId], swapchain.images);
	}

	void update(){
		static int counter = 1;
		//if(counter==KernelRunNumber) NeedToExit = true;
		counter++;
		
		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
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
			renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);

			//std::vector<std::vector<VkDescriptorSet>> dsSets; 
			//dsSets.push_back(computeDescriptorManager.descriptorSets);

			//renderer.BindComputeDescriptorSets(renderProcess.computePipelineLayout, dsSets, -1); //-1 to offset means no dynamic offset


            recordImageBarrier(commandBuffers[i], swapChainImages[i],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            
			
            //vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
            //vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
            //vkCmdDispatch(commandBuffers[i], 200, 300, 1);
			//renderer.Dispatch(300, 600, 1); 
			CSupervisor::Dispatch(300,600,1);
         
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

