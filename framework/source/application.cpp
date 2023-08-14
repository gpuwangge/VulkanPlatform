#include "../include/application.h"

Camera CApplication::mainCamera;  //define static class members
bool CApplication::NeedToExit = false; 

CApplication::CApplication(){
    //debugger = new CDebugger("../logs/application.log");

    mainCamera.type = Camera::CameraType::firstperson;
    mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    mainCamera.movementSpeed = 1.0f;
    mainCamera.rotationSpeed = 200.0f;

    //NeedToExit = false;
    windowWidth = 0;
    windowHeight = 0;
}

#ifndef ANDROID
void CApplication::run(){ //Entrance Function
    glfwManager.createWindow(windowWidth, windowHeight);
    CContext::Init();

    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> requiredInstanceExtensions;
    glfwManager.getGLFWRequiredInstanceExtensions(requiredInstanceExtensions);
    if (enableValidationLayers) requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    //prepareVulkanDevices();
    instance = std::make_unique<CInstance>(requiredValidationLayers,requiredInstanceExtensions);
    //need instance. Surface is to store view format information for creating swapchain. 
    //Only GLFW knows what kind of surface can be attached to its window.
    glfwManager.createSurface(instance, surface);

    instance->findAllPhysicalDevices();
    CContext::GetHandle().logManager->writeMSG("Surface created!\n");
    CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);
    //App dev can only query properties from physical device, but can not directly operate it
    //App dev operates logical device, can logical device communicate with physical device by command queues
    //App dev will fill command buffer with commands later
    //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);

    swapchain.createSwapchainImages(surface, windowWidth, windowHeight);
	swapchain.createImageViews(VK_IMAGE_ASPECT_COLOR_BIT);

    initialize();

    mainLoop();
}
#endif

void CApplication::initialize(){
    renderer.CreateSyncObjects();
    shaderManager.Destroy();
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
    descriptor.updateVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera);

    renderer.Update();
}

#ifndef ANDROID
void CApplication::mainLoop(){
		while (!glfwWindowShouldClose(glfwManager.window)) {
			glfwPollEvents();

            update();
            if(!descriptor.bUseComputeStorage) renderer.prepareCurrentFrameAndAcquireImageIndex(swapchain);//TODO for test compute shader 
            recordCommandBuffer();
            if(!descriptor.bUseComputeStorage) renderer.drawFrame(swapchain);//TODO for test compute shader
			
            if (NeedToExit) break;
		}

		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
}
#endif

#ifndef ANDROID
void CApplication::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
#endif

void CApplication::CleanUp(){
    swapchain.CleanUp();
    renderProcess.Cleanup();
    descriptor.DestroyAndFree();
    textureImage.Destroy();
    renderer.Destroy();

    vkDestroyDevice(CContext::GetHandle().GetLogicalDevice(), nullptr);

#ifndef ANDROID
    if (enableValidationLayers) 
        DestroyDebugUtilsMessengerEXT(instance->getHandle(), instance->debugMessenger, nullptr);
#endif

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    vkDestroyInstance(instance->getHandle(), nullptr);
    
    CContext::Quit();
}

CApplication::~CApplication(){
    CleanUp();
}

