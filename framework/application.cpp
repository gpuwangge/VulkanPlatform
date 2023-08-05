#include "application.h"

CApplication::CApplication(){
    debugger = new CDebugger("../logs/applicationLog.txt");
    //ubo.model = glm::mat4(1.0f);
    bEnableMSAA = false;
    msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    //bEnableDepthTest = false;

    //bEnableUniform = false;

    //textureSampler = NULL;

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

    CContext::Init();
    CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);

    //App dev can only query properties from physical device, but can not directly operate it
    //App dev operates logical device, can logical device communicate with physical device by command queues
    //App dev will fill command buffer with commands later
    //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
}

void CApplication::Init06CreateCommandPool() {
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

void CApplication::Init06CreateCommandBuffers() {
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


void CApplication::Init12SpirvShader(std::string filename, VkShaderModule * pShaderModule){
    HERE_I_AM("Init12SpirvShader");

    auto shaderCode = readFile(filename.c_str());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(CContext::GetHandle().GetLogicalDevice(), &createInfo, PALLOCATOR, pShaderModule);
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


void CApplication::recordCommandBuffer(){
    //printf("application recordCommandBuffer...\n");
    //std::cout<<"imageIndex: "<<imageIndex<<", ";
    //std::cout<<"currentFrame: "<<currentFrame<<std::endl;

    //recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
}

void CApplication::drawFrame() {
    //printf("currentFrame: %d, ", currentFrame);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //CPU-GPU sync use fence
    VkResult result = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    //if(result == VK_SUCCESS) printf("frame is ready. ");
    //else printf("waiting for frame ready. ");

    //CPU need to know which framebuffer to draw。GPU notify CPU the image is ready or not
    //GPU-GPU sync use semaphore
    //semaphore check if image is ready or not. imageIndex is the ready image.
    result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    //if(result == VK_SUCCESS) printf("acquired next image %d. \n", imageIndex);
    //else printf("Unable to aquire next image. \n");

    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

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
    result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);
}


void CApplication::update(){
    //printf("app update...\n");

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    durationTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = currentTime;

    mainCamera.update(deltaTime);

    descriptor.updateMVPUniformBuffer(currentFrame, durationTime);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CApplication::mainLoop(){
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
            update();
            drawFrame();
			if (NeedToExit) break;
		}

		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
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

    swapchain.CleanUp();

    renderProcess.Cleanup();

    //vkDestroyRenderPass(CContext::GetHandle().GetLogicalDevice(), renderProcess.renderPass, nullptr);

    //vkDestroyPipeline(logicalDevice, pipeline_compute, nullptr);
    //vkDestroyPipelineLayout(logicalDevice, pipelineLayout_compute, nullptr);

    //if(bEnableUniform){
    // for (size_t i = 0; i < uniformBuffers.size(); i++) {
    //     uniformBuffers[i].DestroyAndFree();
    // }
    //}
    indexDataBuffer.DestroyAndFree();
    vertexDataBuffer.DestroyAndFree();

    if(textureImageBuffer.size != (VkDeviceSize)0){
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), textureImageBuffer.image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), textureImageBuffer.deviceMemory, nullptr);
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), textureImageView, nullptr);
    }

    //vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), descriptorPool, nullptr);
    //vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout, nullptr);
    descriptor.DestroyAndFree();

    //vkDestroyPipeline(CContext::GetHandle().GetLogicalDevice(), renderProcess.graphicsPipeline, nullptr);
    //vkDestroyPipelineLayout(CContext::GetHandle().GetLogicalDevice(), renderProcess.pipelineLayout, nullptr);

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
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), msaaColorImageView, nullptr);
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), msaaColorImageBuffer.image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), msaaColorImageBuffer.deviceMemory, nullptr);
    }

    //if(textureSampler) vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), textureSampler, nullptr);

    if(renderProcess.bUseDepthAttachment){
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), depthImageBuffer.image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), depthImageBuffer.deviceMemory, nullptr);
        vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), depthImageView, nullptr);
    }


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        //vkDestroySemaphore(logicalDevice, computeFinishedSemaphores[i], nullptr);
        vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), inFlightFences[i], nullptr);
        //vkDestroyFence(logicalDevice, computeInFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(CContext::GetHandle().GetLogicalDevice(), commandPool, nullptr);

    vkDestroyDevice(CContext::GetHandle().GetLogicalDevice(), nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance->getHandle(), instance->debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    vkDestroyInstance(instance->getHandle(), nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    delete debugger;

    CContext::Quit();
}

