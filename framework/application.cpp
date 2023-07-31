#include "application.h"

CApplication::CApplication(){
    debugger = new CDebugger("../logs/applicationLog.txt");
    ubo.model = glm::mat4(1.0f);
    bEnableMSAA = false;
    msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    bEnableDepthTest = false;

    textureSampler = NULL;

    mainCamera.type = Camera::CameraType::firstperson;
    mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    mainCamera.movementSpeed = 1.0f;
    mainCamera.rotationSpeed = 200.0f;
}

void CApplication::run(){
    prepareGLFW();
    prepareVulkanDevices();
    initialize();
    mainLoop();
}

void CApplication::initialize(){
    createSyncObjects();
}

void CApplication::prepareVulkanDevices(){
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    
    instance = std::make_unique<CInstance>(requiredValidationLayers);

    //need instance. Surface is to store view format information for creating swapchain. 
    //Only GLFW knows what kind of surface can be attached to its window.
    createGLFWSurface();

    instance->findAllPhysicalDevices();
    instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);

    //App dev can only query properties from physical device, but can not directly operate it
    //App dev operates logical device, can logical device communicate with physical device by command queues
    //App dev will fill command buffer with commands later
    instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
}

// void CApplication::Init05CreateVertexBuffer() {
//     HERE_I_AM("Init05CreateVertexBuffer");
//     VkDeviceSize bufferSize = sizeof(vertices3D[0]) * vertices3D.size();

//     //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
//     VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexDataBuffer);//allocate vertexDataBuffer bufferSize(decided by vertices3D) memory
//     REPORT("InitVertexDataBuffer");
//     FillDataBufferHelper(vertexDataBuffer, (void *)(vertices3D.data()));//copy vertices3D to vertexDataBuffer
// }

void CApplication::Init05CreateIndexBuffer() {
    HERE_I_AM("Init05CreateIndexBuffer");
    VkDeviceSize bufferSize = sizeof(indices3D[0]) * indices3D.size();

    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexDataBuffer);
    REPORT("InitIndexDataBuffer");
    FillDataBufferHelper(indexDataBuffer, (void *)(indices3D.data()));
}

void CApplication::Init05CreateUniformBuffers(std::vector<MyBuffer> &_uniformBuffers, std::vector<void*> &_uniformBuffersMapped, VkBufferUsageFlags usage, VkDeviceSize bufferSize) {
    HERE_I_AM("Init05CreateUniformBuffers");

    _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = InitDataBufferHelper(bufferSize, usage, &_uniformBuffers[i]);
        REPORT("InitDataBufferHelper");

        vkMapMemory(LOGICAL_DEVICE, _uniformBuffers[i].deviceMemory, 0, bufferSize, 0, &_uniformBuffersMapped[i]);
    }
}

void CApplication::Init06CreateCommandPool() {
    HERE_I_AM("Init06CommandPools");

    VkResult result = VK_SUCCESS;

    QueueFamilyIndices queueFamilyIndices = instance->pickedPhysicalDevice->get()->findQueueFamilies(surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    //poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();//find a queue family that does graphics
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();//graphics command buffer和compute command buffer都需要这个command pool，所以需要graphics and compute family

    result = vkCreateCommandPool(LOGICAL_DEVICE, &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics command pool!");
    REPORT("vkCreateCommandPool -- Graphics");
}

void CApplication::Init06CreateCommandBuffers() {
    HERE_I_AM("Init06CommandBuffers");

    VkResult result = VK_SUCCESS;

    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    result = vkAllocateCommandBuffers(LOGICAL_DEVICE, &allocInfo, commandBuffers.data());//graphcis recordCommandBuffer需要用这个buffer： recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate command buffers!");
    REPORT("vkAllocateCommandBuffers");
}


void CApplication::Init12SpirvShader(std::string filename, VkShaderModule * pShaderModule){
    HERE_I_AM("Init12SpirvShader");

    auto shaderCode = readFile(filename.c_str());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(LOGICAL_DEVICE, &createInfo, PALLOCATOR, pShaderModule);
    REPORT("vkCreateShaderModule");
    debugger->writeMSG("Shader Module '%s' successfully loaded\n", filename.c_str());

}

void CApplication::createSyncObjects() {
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
        if (vkCreateSemaphore(LOGICAL_DEVICE, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(LOGICAL_DEVICE, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(LOGICAL_DEVICE, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        //if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            //vkCreateFence(logicalDevice, &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            //throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        //}
    }
}


void CApplication::recordCommandBuffer(){
    //printf("application recordCommandBuffer...\n");
    //std::cout<<"imageIndex: "<<imageIndex<<", ";
    //std::cout<<"currentFrame: "<<currentFrame<<std::endl;

    //recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
}
/*
void CApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //Step1
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    // if (bEnableDepthTest) {
    //     clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    //     clearValues[1].depthStencil = { 1.0f, 0 };
    //     renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    //     renderPassInfo.pClearValues = clearValues.data();
    // }
    // else {
        clearValues[0].color = { {  0.0f, 0.0f, 0.0f, 1.0f  } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = clearValues.data();
    //}

    //Step2
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Step3
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    //Step4
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    //compute pipeline的结果在shaderStorageBuffers_compute里面，而其他pipeline使用vertexDataBuffer和indexDataBuffer
    // if (pipelineType == PIPELINE_COMPUTE) {
    //     VkDeviceSize offsets[] = { 0 };
    //     vkCmdBindVertexBuffers(commandBuffer, 0, 1, &shaderStorageBuffers_compute[currentFrame].buffer, offsets);

    //     //Step5
    //     vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);
    // }
    // else {
        VkBuffer vertexBuffers[] = { vertexDataBuffer.buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, indexDataBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        //Descriptor Step 5
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

        //Step5
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices3D.size()), 1, 0, 0, 0);
    //}

    //Step6
    vkCmdEndRenderPass(commandBuffer);

    //Step7
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}*/

void CApplication::drawFrame() {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //CPU-GPU sync use fence
    vkWaitForFences(LOGICAL_DEVICE, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //CPU need to know which framebuffer to draw。GPU notify CPU the image is ready or not
    //GPU-GPU sync use semaphore
    //semaphore check if image is ready or not。imageIndex is the ready image。
    VkResult result = vkAcquireNextImageKHR(LOGICAL_DEVICE, swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    vkResetFences(LOGICAL_DEVICE, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer();

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPresentInfoKHR presentInfo{};

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

    presentInfo.pWaitSemaphores = signalSemaphores;
    
    //GPU read recorded command buffer and execute
    if (vkQueueSubmit(GRAPHICS_QUEUE, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
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
    result = vkQueuePresentKHR(PRESENT_QUEUE, &presentInfo);
}


void CApplication::update(){
    //printf("app update...\n");

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    durationTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = currentTime;

    mainCamera.update(deltaTime);

    updateUniformBuffer(currentFrame, durationTime);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CApplication::mainLoop(){
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
            update();
            drawFrame();
			if (NeedToExit) break;
		}

		vkDeviceWaitIdle(LOGICAL_DEVICE);
}

void CApplication::cleanupSwapChain() {
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(LOGICAL_DEVICE, framebuffer, nullptr);
    }

    for (auto imageView : swapchain.swapChainImageViews) {
        vkDestroyImageView(LOGICAL_DEVICE, imageView, nullptr);
    }

    vkDestroySwapchainKHR(LOGICAL_DEVICE, swapchain.getHandle(), nullptr);
}

void CApplication::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}


CApplication::~CApplication(){
    //clean up
    HERE_I_AM("Clean up application");

    cleanupSwapChain();

    vkDestroyRenderPass(LOGICAL_DEVICE, renderPass, nullptr);

    //vkDestroyPipeline(logicalDevice, pipeline_compute, nullptr);
    //vkDestroyPipelineLayout(logicalDevice, pipelineLayout_compute, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(LOGICAL_DEVICE, uniformBuffers[i].buffer, nullptr);
        vkFreeMemory(LOGICAL_DEVICE, uniformBuffers[i].deviceMemory, nullptr);
    }

    if(textureImageBuffer.size != (VkDeviceSize)0){
        vkDestroyImage(LOGICAL_DEVICE, textureImageBuffer.image, nullptr);
        vkFreeMemory(LOGICAL_DEVICE, textureImageBuffer.deviceMemory, nullptr);
        vkDestroyImageView(LOGICAL_DEVICE, textureImageView, nullptr);
    }

    vkDestroyDescriptorPool(LOGICAL_DEVICE, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(LOGICAL_DEVICE, descriptorSetLayout, nullptr);
    vkDestroyPipeline(LOGICAL_DEVICE, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(LOGICAL_DEVICE, pipelineLayout, nullptr);

    //for (int i = 0; i < 1; i++) {
        //vkDestroyDescriptorPool(LOGICAL_DEVICE, descriptorPool[i], nullptr);
        //vkDestroyDescriptorSetLayout(LOGICAL_DEVICE, descriptorSetLayout[i], nullptr);
        //vkDestroyPipeline(LOGICAL_DEVICE, graphicsPipeline[i], nullptr);
        //vkDestroyPipelineLayout(LOGICAL_DEVICE, pipelineLayout[i], nullptr);

        //vkDestroyImage(logicalDevice, textureImageBuffer[i].image, nullptr);
        //vkFreeMemory(logicalDevice, textureImageBuffer[i].deviceMemory, nullptr);
        //vkDestroyImageView(logicalDevice, textureImageView[i], nullptr);
    //}
    // for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {
    //     vkDestroyImage(logicalDevice, textureImageBuffers_mipmap[i].image, nullptr);
    //     vkFreeMemory(logicalDevice, textureImageBuffers_mipmap[i].deviceMemory, nullptr);
    // }
    if(bEnableMSAA){
        vkDestroyImageView(LOGICAL_DEVICE, msaaColorImageView, nullptr);
        vkDestroyImage(LOGICAL_DEVICE, msaaColorImageBuffer.image, nullptr);
        vkFreeMemory(LOGICAL_DEVICE, msaaColorImageBuffer.deviceMemory, nullptr);
    }

    if(textureSampler) vkDestroySampler(LOGICAL_DEVICE, textureSampler, nullptr);

    if(bEnableDepthTest){
        vkDestroyImage(LOGICAL_DEVICE, depthImageBuffer.image, nullptr);
        vkFreeMemory(LOGICAL_DEVICE, depthImageBuffer.deviceMemory, nullptr);
        vkDestroyImageView(LOGICAL_DEVICE, depthImageView, nullptr);
    }

    vkDestroyBuffer(LOGICAL_DEVICE, indexDataBuffer.buffer, nullptr);
    vkFreeMemory(LOGICAL_DEVICE, indexDataBuffer.deviceMemory, nullptr);

    vkDestroyBuffer(LOGICAL_DEVICE, vertexDataBuffer.buffer, nullptr);
    vkFreeMemory(LOGICAL_DEVICE, vertexDataBuffer.deviceMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(LOGICAL_DEVICE, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(LOGICAL_DEVICE, imageAvailableSemaphores[i], nullptr);
        //vkDestroySemaphore(logicalDevice, computeFinishedSemaphores[i], nullptr);
        vkDestroyFence(LOGICAL_DEVICE, inFlightFences[i], nullptr);
        //vkDestroyFence(logicalDevice, computeInFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(LOGICAL_DEVICE, commandPool, nullptr);

    vkDestroyDevice(LOGICAL_DEVICE, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance->getHandle(), instance->debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    vkDestroyInstance(instance->getHandle(), nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    delete debugger;
}

void CApplication::updateUniformBuffer(uint32_t currentFrame, float durationTime) {
    // UniformBufferObject ubo{};
    // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // glm::mat4x4 m = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 10.0f);
    // m[1][1] *= -1;
    // ubo.proj = m;
    // ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    //UniformBufferObject ubo{};
    ubo.view = mainCamera.matrices.view;
    ubo.proj = mainCamera.matrices.perspective;
    // if (pipelineType == PIPELINE_MIPMAP) {
            //ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // }
    // else {
        //ubo.model = glm::rotate(glm::mat4(1.0f), durationTime * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //}

    memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));

    //CApplication::updateUniformBuffer(currentFrame, durationTime);
}