#include "renderer.h"

CRenderer::CRenderer(){
    currentFrame = 1;
    imageIndex = 0;
    debugger = new CDebugger("../logs/renderer.log");
}
CRenderer::~CRenderer(){if (!debugger) delete debugger;}

void CRenderer::Update(){
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CRenderer::CreateIndexBuffer(std::vector<uint32_t> &indices3D){
    //Init05CreateIndexBuffer();

	HERE_I_AM("wxjCreateIndexBuffer");
    VkDeviceSize bufferSize = sizeof(indices3D[0]) * indices3D.size();

    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexDataBuffer);
    VkResult result = indexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
 
	REPORT("InitIndexDataBuffer");
    //FillDataBufferHelper(indexDataBuffer, (void *)(indices3D.data()));
	indexDataBuffer.fill((void *)(indices3D.data()));
}

void CRenderer::InitCreateCommandPool(VkSurfaceKHR &surface) {
    HERE_I_AM("Init06CommandPools");

    VkResult result = VK_SUCCESS;

    //QueueFamilyIndices queueFamilyIndices = instance->pickedPhysicalDevice->get()->findQueueFamilies(surface);
    QueueFamilyIndices queueFamilyIndices = CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    //poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();//find a queue family that does graphics
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

    result = vkCreateCommandPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics command pool!");
    REPORT("vkCreateCommandPool -- Graphics");
}

void CRenderer::InitCreateCommandBuffers() {
    HERE_I_AM("Init06CommandBuffers");

    VkResult result = VK_SUCCESS;

    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    result = vkAllocateCommandBuffers(CContext::GetHandle().GetLogicalDevice(), &allocInfo, commandBuffers.data());

    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate command buffers!");
    REPORT("vkAllocateCommandBuffers");
}

void CRenderer::prepareCurrentFrameAndAcquireImageIndex(CSwapchain &swapchain){
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

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
}

void CRenderer::drawFrame(CSwapchain &swapchain) {
    //recordCommandBuffer();//TODO
    //printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkPresentInfoKHR presentInfo{};
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    //GPU read recorded command buffer and execute
    if (vkQueueSubmit(CContext::GetHandle().GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        debugger->writeMSG("Failed to submit draw command buffer! CurrentFrame: %d\n", currentFrame);
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

void CRenderer::BeginCommandBuffer(){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //Step1
    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
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
    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void CRenderer::BindPipeline(VkPipeline &pipeline){
	vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline); //renderProcess.graphicsPipeline
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
	vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);
}
void CRenderer::SetScissor(VkExtent2D &extent){
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent; //swapchain.swapChainExtent;
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
}
void CRenderer::BindVertexBuffer(){
	VkBuffer vertexBuffers[] = {vertexDataBuffer.buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
}
void CRenderer::BindIndexBuffer(){
	vkCmdBindIndexBuffer(commandBuffers[currentFrame], indexDataBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}
void CRenderer::BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<VkDescriptorSet> &descriptorSets){//renderProcess.pipelineLayout, &descriptor.descriptorSets[currentFrame]
	vkCmdBindDescriptorSets(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
}
void CRenderer::DrawIndexed(std::vector<uint32_t> &indices3D){
	vkCmdDrawIndexed(commandBuffers[currentFrame], static_cast<uint32_t>(indices3D.size()), 1, 0, 0, 0);
}
void CRenderer::Draw(uint32_t n){
	vkCmdDraw(commandBuffers[currentFrame], n, 1, 0, 0);
}
void CRenderer::EndRenderPass(){
	vkCmdEndRenderPass(commandBuffers[currentFrame]);
}
void CRenderer::EndCOmmandBuffer(){
	if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

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