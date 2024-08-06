#include "..\\framework\\include\\application.h"

#define TEST_CLASS_NAME CSimpleComputeStorageImage
class TEST_CLASS_NAME: public CApplication{
//This test draws on the swapchain images, with the use of storage image
//The size of swapchain must be equal to MAX_FRAMES_IN_FLIGHT
//Because number of descriptor is MAX_FRAMES_IN_FLIGHT, each swapchain imageview must attach to a descriptor
//Thus no graphics pipeline is needed here
//When run, a purple rectangle will show on the screen

	//static const int KernelRunNumber = 1;
public:
	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	
	CSimpleComputeStorageImage(){
		swapchain.imageSize = MAX_FRAMES_IN_FLIGHT;
		swapchain.bComputeSwapChainImage = true;
	}

	void initialize(){
		renderer.m_renderMode = renderer.RENDER_COMPUTE_SWAPCHAIN_Mode;

		renderer.CreateCommandPool(surface);
		renderer.CreateComputeCommandBuffer();


		shaderManager.CreateShader("simpleComputeStorageImage/comp.spv", shaderManager.COMP);
		std::cout<<"compute shader created."<<std::endl;

		CComputeDescriptorManager::addStorageImage(UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT); //as output
		std::cout<<"descriptor 1."<<std::endl;
		CDescriptorManager::createDescriptorPool();
		std::cout<<"descriptor 2."<<std::endl;
		CComputeDescriptorManager::createDescriptorSetLayout();
		std::cout<<"descriptor 3."<<std::endl;
		computeDescriptorManager.createDescriptorSets(NULL, &(swapchain.views));
		std::cout<<"descriptor created."<<std::endl;

		///renderProcess.createGraphicsPipelineLayout(descriptors[0].descriptorSetLayout);
		///renderProcess.createGraphicsPipeline(
			///VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			///shaderManager.vertShaderModule, 
			///shaderManager.fragShaderModule);

		renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
		renderProcess.createComputePipeline(shaderManager.compShaderModule);

		CApplication::initialize();
		createComputeCommandBuffers(renderer.commandBuffers[renderer.computeCmdId], swapchain.images);
	}

	void update(){
		CApplication::update();

		//static int counter = 1;
		//if(counter==KernelRunNumber) NeedToExit = true;
		//counter++;
	}

	void recordComputeCommandBuffer(){
		//std::cout<<"Record Compute command buffer. "<<std::endl;

		
	//command buffer recorded in init() so dont need to record here
/*
		START_COMPUTE_RECORD(0)

		recordImageBarrier(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], swapchain.images[renderer.currentFrame],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

		renderer.Dispatch(200, 300, 1); 

		recordImageBarrier(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], swapchain.images[renderer.currentFrame],
                VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		END_COMPUTE_RECORD*/
	}

	///void recordGraphicsCommandBuffer(){
		///START_GRAPHICS_RECORD(0)
		
		///renderer.Draw(3);
		
		///END_GRAPHICS_RECORD
	///}

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

			std::vector<std::vector<VkDescriptorSet>> dsSets; 
			dsSets.push_back(computeDescriptorManager.descriptorSets);

			renderer.BindComputeDescriptorSets(renderProcess.computePipelineLayout, dsSets, -1); //-1 to offset means no dynamic offset

            recordImageBarrier(commandBuffers[i], swapChainImages[i],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            
			
            //vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
            //vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
            //vkCmdDispatch(commandBuffers[i], 200, 300, 1);
			renderer.Dispatch(200, 300, 1); 
         
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

