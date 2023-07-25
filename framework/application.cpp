#include "application.h"

CApplication::CApplication(){
    debugger = new CDebugger("applicationLog.txt");
}

void CApplication::run(){
    prepareGLFW();
    initVulkan();
    mainLoop();
}

void CApplication::initVulkan(){
    printf("application init\n");
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    
    instance = std::make_unique<CInstance>(requiredValidationLayers);
    createGLFWSurface();//need instance
    instance->findAllPhysicalDevices();
    instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);
    instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);

    Init05CreateVertexBuffer();
    Init05CreateIndexBuffer();
    Init05CreateUniformBuffers(uniformBuffers, uniformBuffersMapped, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject));

	Init06CreateCommandPool();
	Init06CreateCommandBuffers();

    if(!texturePath.empty()){
        CreateImageTexture();
	    textureImageView = createImageView(textureImageBuffer.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    Init08CreateSwapChain();

    CreateRenderPass();

    CreateFramebuffers();

    Init12SpirvShader(vertexShaderPath, &vertShaderModule);
	Init12SpirvShader(fragmentShaderPath, &fragShaderModule);

    CreateDescriptorPool();
    CreateDescriptorSetLayout();
    CreateDescriptorSets();
    CreateGraphicsPipeline();

    createSyncObjects();
}

void CApplication::Init05CreateVertexBuffer() {
    HERE_I_AM("Init05CreateVertexBuffer");
    VkDeviceSize bufferSize = sizeof(vertices3D[0]) * vertices3D.size();

    VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &vertexDataBuffer);//为vertexDataBuffer申请bufferSize(由vertices3D确定)大小的memory
    REPORT("InitVertexDataBuffer");
    FillDataBufferHelper(vertexDataBuffer, (void *)(vertices3D.data()));//把vertices3D的内容拷贝到vertexDataBuffer里
}

void CApplication::Init05CreateIndexBuffer() {
    HERE_I_AM("Init05CreateIndexBuffer");
    VkDeviceSize bufferSize = sizeof(indices3D[0]) * indices3D.size();

    VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &indexDataBuffer);
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

void CApplication::Init08CreateSwapChain() {
    HERE_I_AM("Init08Swapchain");

    VkResult result = VK_SUCCESS;

    SwapChainSupportDetails swapChainSupport = instance->pickedPhysicalDevice->get()->querySwapChainSupport(surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = instance->pickedPhysicalDevice->get()->findQueueFamilies(surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    //生成swapChain
    result = vkCreateSwapchainKHR(LOGICAL_DEVICE, &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create swap chain!");
    REPORT("vkCreateSwapchainKHR");

    result = vkGetSwapchainImagesKHR(LOGICAL_DEVICE, swapChain, &imageCount, nullptr);
    REPORT("vkGetSwapchainImagesKHR - 0 (Get imageCount)");
    swapChainImages.resize(imageCount);
    result = vkGetSwapchainImagesKHR(LOGICAL_DEVICE, swapChain, &imageCount, swapChainImages.data());
    REPORT("vkGetSwapchainImagesKHR - 1");

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    // present views for the double-buffering:
    swapChainImageViews.resize(swapChainImages.size());

    //生成swapChainImageViews
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        REPORT("vkCreateImageView");
    }
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

    //compute piepline的结果在shaderStorageBuffers_compute里面，而其他pipeline使用vertexDataBuffer和indexDataBuffer
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

}

void CApplication::drawFrame() {
    //printf("app drawFrame...\n");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //=======================Compute submission========================
    // if (pipelineType == PIPELINE_COMPUTE) {
    //     vkWaitForFences(logicalDevice, 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //     updateUniformBuffer_compute(currentFrame);

    //     vkResetFences(logicalDevice, 1, &computeInFlightFences[currentFrame]);

    //     vkResetCommandBuffer(commandBuffers_compute[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    //     recordComputeCommandBuffer(commandBuffers_compute[currentFrame]); //Dispatch in this function

    //     submitInfo.commandBufferCount = 1;
    //     submitInfo.pCommandBuffers = &commandBuffers_compute[currentFrame];
    //     submitInfo.signalSemaphoreCount = 1;
    //     submitInfo.pSignalSemaphores = &computeFinishedSemaphores[currentFrame];

    //     if (vkQueueSubmit(computeQueue, 1, &submitInfo, computeInFlightFences[currentFrame]) != VK_SUCCESS) {
    //         throw std::runtime_error("failed to submit compute command buffer!");
    //     };
    // }
    //=======================Graphics submission========================
    vkWaitForFences(LOGICAL_DEVICE, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(LOGICAL_DEVICE, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //recreateSwapChain();//TODO
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    //if (pipelineType != PIPELINE_COMPUTE) {
        //updateUniformBuffer(currentFrame); //目前pariticle system不需要更新MVP,移动此函数到update()
    //}

    vkResetFences(LOGICAL_DEVICE, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //NTODO: present and Semaphore
    VkPresentInfoKHR presentInfo{};
    // if (pipelineType == PIPELINE_COMPUTE) {
    //     VkSemaphore waitSemaphores[] = { computeFinishedSemaphores[currentFrame], imageAvailableSemaphores[currentFrame] };
    //     VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    //     submitInfo = {};
    //     submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //     submitInfo.waitSemaphoreCount = 2;
    //     submitInfo.pWaitSemaphores = waitSemaphores;
    //     submitInfo.pWaitDstStageMask = waitStages;
    //     submitInfo.commandBufferCount = 1;
    //     submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    //     submitInfo.signalSemaphoreCount = 1;
    //     submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

    //     presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
    // }
    // else {
        //VkSemaphore waitSemaphores[] = { computeFinishedSemaphores[currentFrame],  imageAvailableSemaphores[currentFrame] };
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
    //}

    if (vkQueueSubmit(GRAPHICS_QUEUE, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        debugger->writeMSG("Failed to submit draw command buffer! CurrentFrame: %d\n", currentFrame);
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;


    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(PRESENT_QUEUE, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        //recreateSwapChain();//TODO
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

void CApplication::updateUniformBuffer(uint32_t currentFrame, float durationTime) {

}

void CApplication::update(){
    //printf("app update...\n");

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float durationTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = currentTime;

    // We want to animate the particle system using the last frames time to get smooth, frame-rate independent animation
    //double currentTime = glfwGetTime();
    //elapsedTime = (currentTime - lastTime) * 1000.0;//这个值会用来update deltaTime
    //lastTime = currentTime.count();

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

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(LOGICAL_DEVICE, imageView, nullptr);
    }

    vkDestroySwapchainKHR(LOGICAL_DEVICE, swapChain, nullptr);
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

    vkDestroyDescriptorPool(LOGICAL_DEVICE, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(LOGICAL_DEVICE, descriptorSetLayout, nullptr);
    vkDestroyPipeline(LOGICAL_DEVICE, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(LOGICAL_DEVICE, pipelineLayout, nullptr);

    if(!texturePath.empty()){
        vkDestroyImage(LOGICAL_DEVICE, textureImageBuffer.image, nullptr);
        vkFreeMemory(LOGICAL_DEVICE, textureImageBuffer.deviceMemory, nullptr);
        vkDestroyImageView(LOGICAL_DEVICE, textureImageView, nullptr);
    }
    
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
    //vkDestroyImageView(logicalDevice, colorImageView_msaa, nullptr);
    //vkDestroyImage(logicalDevice, colorImageBuffer_msaa.image, nullptr);
    //vkFreeMemory(logicalDevice, colorImageBuffer_msaa.deviceMemory, nullptr);

    //vkDestroySampler(logicalDevice, textureSampler, nullptr);

    //vkDestroyImage(logicalDevice, depthImageBuffer.image, nullptr);
    //vkFreeMemory(logicalDevice, depthImageBuffer.deviceMemory, nullptr);
    //vkDestroyImageView(logicalDevice, depthImageView, nullptr);

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

/*TODO
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);*/

    glfwDestroyWindow(window);

    glfwTerminate();

    delete debugger;
}