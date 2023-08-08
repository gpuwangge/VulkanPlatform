#include "application.h"

CApplication::CApplication(){
    debugger = new CDebugger("../logs/application.log");

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
    renderer.CreateSyncObjects();

    shaderManager.Destroy();
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


void CApplication::recordCommandBuffer(){

}




void CApplication::update(){
    //printf("app update...\n");

    static auto startTime = std::chrono::high_resolution_clock::now();
    static auto lastTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    durationTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = currentTime;

    mainCamera.update(deltaTime);

    descriptor.updateMVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera);

    renderer.Update();
}

void CApplication::mainLoop(){
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
            update();
            renderer.prepareCurrentFrameAndAcquireImageIndex(swapchain);
            recordCommandBuffer();
            renderer.drawFrame(swapchain);
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
    //indexDataBuffer.DestroyAndFree();
    //vertexDataBuffer.DestroyAndFree();

    // if(textureImageBuffer.size != (VkDeviceSize)0){
    //     vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), textureImageBuffer.image, nullptr);
    //     vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), textureImageBuffer.deviceMemory, nullptr);
    //     vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), textureImageView, nullptr);
    // }

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
    // if(bEnableMSAA){
    //     vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), msaaColorImageView, nullptr);
    //     vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), msaaColorImageBuffer.image, nullptr);
    //     vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), msaaColorImageBuffer.deviceMemory, nullptr);
    // }
    

    //if(textureSampler) vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), textureSampler, nullptr);

    // if(renderProcess.bUseDepthAttachment){
    //     vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), depthImageBuffer.image, nullptr);
    //     vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), depthImageBuffer.deviceMemory, nullptr);
    //     vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), depthImageView, nullptr);
    // }
    textureImage.Destroy();


    // for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    //     vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
    //     vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
    //     //vkDestroySemaphore(logicalDevice, computeFinishedSemaphores[i], nullptr);
    //     vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), inFlightFences[i], nullptr);
    //     //vkDestroyFence(logicalDevice, computeInFlightFences[i], nullptr);
    // }

    // vkDestroyCommandPool(CContext::GetHandle().GetLogicalDevice(), commandPool, nullptr);
    renderer.Destroy();

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

