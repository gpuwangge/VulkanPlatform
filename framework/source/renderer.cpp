#include "../include/renderer.h"

CRenderer::CRenderer(){
    currentFrame = 0;
    imageIndex = 0;
    //debugger = new CDebugger("../logs/renderer.log");
    graphicsCmdId = 0;
    computeCmdId = 0;
}
CRenderer::~CRenderer(){
    //if (!debugger) delete debugger;
}

void CRenderer::Update(){
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CRenderer::CreateIndexBuffer(std::vector<uint32_t> &indices3D){
    //Init05CreateIndexBuffer();

	//HERE_I_AM("wxjCreateIndexBuffer");
    VkDeviceSize bufferSize = sizeof(indices3D[0]) * indices3D.size();

    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexDataBuffer);
    VkResult result = indexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
 
	//REPORT("InitIndexDataBuffer");
    //FillDataBufferHelper(indexDataBuffer, (void *)(indices3D.data()));
	indexDataBuffer.fill((void *)(indices3D.data()));
}


/**************************
 * 
 * Command Buffer Functions
 * 
 * ***********************/
void CRenderer::CreateCommandPool(VkSurfaceKHR &surface) {
    //HERE_I_AM("Init06CommandPools");

    VkResult result = VK_SUCCESS;

    //QueueFamilyIndices queueFamilyIndices = instance->pickedPhysicalDevice->get()->findQueueFamilies(surface);
    QueueFamilyIndices queueFamilyIndices = CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, "Find Queue Families when creating command pool");

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    //poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();//find a queue family that does graphics
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

    result = vkCreateCommandPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics command pool!");
    //REPORT("vkCreateCommandPool -- Graphics");
}

void CRenderer::CreateGraphicsCommandBuffer(){
    graphicsCmdId = commandBuffers.size();
    CreateCommandBuffers();
}
void CRenderer::CreateComputeCommandBuffer(){
    computeCmdId = commandBuffers.size();
    CreateCommandBuffers();
}
void CRenderer::CreateCommandBuffers() {
    //commandBuffers.resize(size); //if enable both graphics and compute pipelines, set 2 commandBuffers: 0-GRAPHCIS, 1-COMPUTE
    std::vector<VkCommandBuffer> commandBuffer;

    //for(int i = 0; i < size; i++){
    VkResult result = VK_SUCCESS;

    commandBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    //commandBuffer.resize(3);///!!!

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffer.size();

    result = vkAllocateCommandBuffers(CContext::GetHandle().GetLogicalDevice(), &allocInfo, commandBuffer.data());

    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate command buffers!");
    //REPORT("vkAllocateCommandBuffers");
    //}

    commandBuffers.push_back(commandBuffer);
}

// void CRenderer::CreateComputeCommandBuffers() {
// 	commandBuffers_compute.resize(MAX_FRAMES_IN_FLIGHT);

// 	VkCommandBufferAllocateInfo allocInfo{};
// 	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
// 	allocInfo.commandPool = commandPool;
// 	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
// 	allocInfo.commandBufferCount = (uint32_t)commandBuffers_compute.size();

// 	if (vkAllocateCommandBuffers(CContext::GetHandle().GetLogicalDevice(), &allocInfo, commandBuffers_compute.data()) != VK_SUCCESS) { //compute recordCommandBuffer需要用这个buffer： recordCompute CommandBuffer(commandBuffers_compute[currentFrame]);
// 		throw std::runtime_error("failed to allocate compute command buffers!");
// 	}
// }


/**************************
 * 
 * Graphics Functions
 * 
 * ***********************/

void CRenderer::preRecordGraphicsCommandBuffer(CSwapchain &swapchain){//prepareCurrentFrameAndAcquireImageIndex
    //printf("currentFrame: %d, ", currentFrame);

    // VkSubmitInfo submitInfo{};
    // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //CPU-GPU sync use fence
    VkResult result = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    //if(result == VK_SUCCESS) printf("frame is ready. ");
    //else printf("waiting for frame ready. ");

    //CPU need to know which framebuffer to draw,GPU notify CPU the image is ready or not
    //GPU-GPU sync use semaphore
    //semaphore check if image is ready or not. imageIndex is the ready image.
    result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    //if(result == VK_SUCCESS) printf("acquired next image %d. \n", imageIndex);
    //else printf("Unable to aquire next image. \n");

    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[graphicsCmdId][currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
}

void CRenderer::postRecordGraphicsCommandBuffer(CSwapchain &swapchain) {//drawFrame
    //printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[graphicsCmdId][currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkPresentInfoKHR presentInfo{};
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    //GPU read recorded command buffer and execute
    if (vkQueueSubmit(CContext::GetHandle().GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        //debugger->writeMSG("Failed to submit draw command buffer! CurrentFrame: %d\n", currentFrame);
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;


    VkSwapchainKHR swapChains[] = { swapchain.getHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    //present engine read image the present(present imageIndex)
    VkResult result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);
}

void CRenderer::CreateSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    //computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    //computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    //imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
    imagesInFlight.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    //imagesInFlight.resize(3, VK_NULL_HANDLE);///!!!

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(CContext::GetHandle().GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        //if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            //vkCreateFence(logicalDevice, &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            //throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        //}
    }
}


void CRenderer::StartRecordGraphicsCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout, 
        VkRenderPass &renderPass, 
        std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent,
        std::vector<VkDescriptorSet> &descriptorSets,
        std::vector<VkClearValue> &clearValues){
    //std::cout<<"start record start"<<std::endl;
    BeginCommandBuffer(graphicsCmdId);
    //std::cout<<"BeginCommandBuffer done"<<std::endl;
    BeginRenderPass(renderPass, swapChainFramebuffers, extent, clearValues);
    //std::cout<<"BeginRenderPass done"<<std::endl;
    BindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsCmdId);
    //std::cout<<"BindPipeline done"<<std::endl;
    SetViewport(extent);
    SetScissor(extent);
    BindDescriptorSets(pipelineLayout, descriptorSets, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsCmdId);
    //std::cout<<"start record done"<<std::endl;
}
void CRenderer::EndRecordGraphicsCommandBuffer(){
	EndRenderPass();
	EndCommandBuffer(graphicsCmdId);
}

void CRenderer::BeginCommandBuffer(int commandBufferIndex){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //Step1
    if (vkBeginCommandBuffer(commandBuffers[commandBufferIndex][currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
        std::cout<<"failed to begin recording command buffer!"<<std::endl;
    }
}
void CRenderer::BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues){
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = extent;//swapchain.swapChainExtent;

    //std::array<VkClearValue, 2> clearValues{};
    // if (bEnableDepthTest) {
        // clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        // clearValues[1].depthStencil = { 1.0f, 0 };
        // renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        // renderPassInfo.pClearValues = clearValues.data();
    // }
    // else {
    	//clearValues[0].color = { {  0.0f, 0.0f, 0.0f, 1.0f  } };
    	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    	renderPassInfo.pClearValues = clearValues.data();
    //}

    //Step2
    vkCmdBeginRenderPass(commandBuffers[graphicsCmdId][currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void CRenderer::BindPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex){
	vkCmdBindPipeline(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipeline); //renderProcess.graphicsPipeline
}
void CRenderer::SetViewport(VkExtent2D &extent){
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = extent.width; //(float)swapchain.swapChainExtent.width;
	viewport.height = extent.height; //(float)swapchain.swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	//Step4
	vkCmdSetViewport(commandBuffers[graphicsCmdId][currentFrame], 0, 1, &viewport);
}
void CRenderer::SetScissor(VkExtent2D &extent){
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent; //swapchain.swapChainExtent;
    vkCmdSetScissor(commandBuffers[graphicsCmdId][currentFrame], 0, 1, &scissor);
}
void CRenderer::BindVertexBuffer(){
	VkBuffer vertexBuffers[] = {vertexDataBuffer.buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[graphicsCmdId][currentFrame], 0, 1, vertexBuffers, offsets);
}
void CRenderer::BindIndexBuffer(){
	vkCmdBindIndexBuffer(commandBuffers[graphicsCmdId][currentFrame], indexDataBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}
void CRenderer::BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<VkDescriptorSet> &descriptorSets, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex){
	vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
}

void CRenderer::DrawIndexed(std::vector<uint32_t> &indices3D){
	vkCmdDrawIndexed(commandBuffers[graphicsCmdId][currentFrame], static_cast<uint32_t>(indices3D.size()), 1, 0, 0, 0);
}
void CRenderer::Draw(uint32_t n){
	vkCmdDraw(commandBuffers[graphicsCmdId][currentFrame], n, 1, 0, 0);
}
void CRenderer::EndRenderPass(){
	vkCmdEndRenderPass(commandBuffers[graphicsCmdId][currentFrame]);
}
void CRenderer::EndCommandBuffer(int commandBufferIndex){
	if (vkEndCommandBuffer(commandBuffers[commandBufferIndex][currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

/**************************
 * 
 * Compute Shader Functions
 * 
 * ***********************/

void CRenderer::preRecordComputeCommandBuffer(CSwapchain &swapchain){ //prepareCurrentFrame
   
    vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

     printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[computeCmdId][currentFrame];///!!!

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(CContext::GetHandle().GetComputeQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain.getHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);


/*
    VkResult result = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);//

    //updateUniformBuffer_compute(currentFrame);
    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);
    //vkResetCommandBuffer(commandBuffers[graphicsCmdId][currentFrame],  0);////////////////////
    */
   /*VkCommandBufferResetFlagBits*/
}
void CRenderer::postRecordComputeCommandBuffer(CSwapchain &swapchain){
    /*
    printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };//
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };//
    submitInfo.waitSemaphoreCount = 1;//
    submitInfo.pWaitSemaphores = waitSemaphores;//
    submitInfo.pWaitDstStageMask = waitStages;//
    
    submitInfo.commandBufferCount = 1;
    //submitInfo.pCommandBuffers = &commandBuffers[computeCmdId][currentFrame];
    submitInfo.pCommandBuffers = &commandBuffers[computeCmdId][imageIndex];//

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };//
    submitInfo.signalSemaphoreCount = 1;//
    submitInfo.pSignalSemaphores = signalSemaphores;//
    //submitInfo.signalSemaphoreCount = 1;//
    //submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];//

    if (vkQueueSubmit(CContext::GetHandle().GetComputeQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit compute command buffer!");
    }; 


    VkPresentInfoKHR presentInfo{};//
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;//

    //VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };//
    presentInfo.waitSemaphoreCount = 1;//
    presentInfo.pWaitSemaphores = signalSemaphores;//

    VkSwapchainKHR swapChains[] = { swapchain.getHandle() };//
    presentInfo.swapchainCount = 1;//
    presentInfo.pSwapchains = swapChains;//

    presentInfo.pImageIndices = &imageIndex;//

    VkResult result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);//
    */
}

void CRenderer::StartRecordComputeCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout, std::vector<VkDescriptorSet> &descriptorSets){
    BeginCommandBuffer(computeCmdId);
    //BeginRenderPass(renderPass, swapChainFramebuffers, extent, clearValues);
    BindPipeline(pipeline, VK_PIPELINE_BIND_POINT_COMPUTE, computeCmdId);
    //SetViewport(extent);
    //SetScissor(extent);
    BindDescriptorSets(pipelineLayout, descriptorSets, VK_PIPELINE_BIND_POINT_COMPUTE, computeCmdId);
}
void CRenderer::EndRecordComputeCommandBuffer(){
	//EndRenderPass();
	EndCommandBuffer(computeCmdId);
}

void CRenderer::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    vkCmdDispatch(commandBuffers[computeCmdId][currentFrame], numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ); 
}


/**************************
 * 
 * Clean up Function
 * 
 * ***********************/

void CRenderer::Destroy(){
    indexDataBuffer.DestroyAndFree();
    vertexDataBuffer.DestroyAndFree();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        //vkDestroySemaphore(logicalDevice, computeFinishedSemaphores[i], nullptr);
        vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), inFlightFences[i], nullptr);
        //vkDestroyFence(logicalDevice, computeInFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(CContext::GetHandle().GetLogicalDevice(), commandPool, nullptr);
}



