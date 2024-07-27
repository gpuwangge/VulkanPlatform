#include "../include/application.h"

Camera CApplication::mainCamera;  //define static class members
bool CApplication::NeedToExit = false; 

CApplication::CApplication(){
    //debugger = new CDebugger("../logs/application.log");

    descriptors.resize(1);
    textureImages.resize(1);

    mainCamera.type = Camera::CameraType::firstperson;
    mainCamera.setPosition(glm::vec3(0.0f, 0.0f, -2.0f));
    mainCamera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    mainCamera.setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    mainCamera.movementSpeed = 1.0f;
    mainCamera.rotationSpeed = 200.0f;

    //NeedToExit = false;
    windowWidth = 0;
    windowHeight = 0;
}

#ifndef ANDROID
void CApplication::run(){ //Entrance Function
    CContext::Init();

    glfwManager.createWindow(OUT windowWidth, OUT windowHeight);

	PRINT("run: Created Window. Window width = %d,  height = %d.", windowWidth, windowHeight);

    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> requiredInstanceExtensions;
    glfwManager.getGLFWRequiredInstanceExtensions(requiredInstanceExtensions);
    if(enableValidationLayers) requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    //prepareVulkanDevices();
    instance = std::make_unique<CInstance>(requiredValidationLayers,requiredInstanceExtensions);
    //need instance. Surface is to store view format information for creating swapchain. 
    //Only GLFW knows what kind of surface can be attached to its window.
    glfwManager.createSurface(instance, surface);

    instance->findAllPhysicalDevices();

    CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);
    //App dev can only query properties from physical device, but can not directly operate it
    //App dev operates logical device, can logical device communicate with physical device by command queues
    //App dev will fill command buffer with commands later
    //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);

    //query  basic capabilities of surface
    //VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities;
    //std::cout<<vkGetPhysicalDeviceSurfaceCapabilitiesKHR(CContext::GetHandle().GetPhysicalDevice(), surface, pSurfaceCapabilities)<<std::endl;
    //std::cout<<"Surface min extent: width="<<pSurfaceCapabilities->minImageExtent.width<<", Surface min extent: height="<<pSurfaceCapabilities->minImageExtent.height<<std::endl;
    //std::cout<<"Surface max extent: width="<<pSurfaceCapabilities->maxImageExtent.width<<", Surface max extent: height="<<pSurfaceCapabilities->maxImageExtent.height<<std::endl;

    swapchain.createImages(surface, windowWidth, windowHeight);
	swapchain.createImageViews(VK_IMAGE_ASPECT_COLOR_BIT);

    initialize();

    while (!glfwWindowShouldClose(glfwManager.window)) {
        glfwPollEvents();
        UpdateRecordRender();
        if (NeedToExit) break;
	}

	vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
}
#endif

void CApplication::initialize(){
    renderer.CreateSyncObjects(swapchain.imageSize);
    shaderManager.Destroy();

    // CContext::GetHandle().logManager.print("Test single string!\n");
    // CContext::GetHandle().logManager.print("Test interger: %d!\n", 999);
    // CContext::GetHandle().logManager.print("Test float: %f!\n", 1.234f);
    // CContext::GetHandle().logManager.print("Test string: %s!\n", "another string");
    // float mat[4] = {1.1, 2.2, 3.3, 4.4};
    // CContext::GetHandle().logManager.print("Test vector: \n", mat, 4);
    // CContext::GetHandle().logManager.print("Test two floats:  %f, %f!\n", 1.2, 2.3);

    // PRINT("Test single string!");
    // PRINT("Test interger: %d!", 999);
    // PRINT("Test float: %f!", 1.234f);
    // PRINT("Test string: %s!", "another string");
    // float mat[4] = {1.1, 2.2, 3.3, 4.4};
    // PRINT("Test vector: ", mat, 4);
    // PRINT("Test two floats:  %f, %f!", 1.2, 2.3);    
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

    for(int i = 0; i < descriptors.size(); i++){
        descriptors[i].updateMVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera);
        descriptors[i].updateVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera);
    }
}

void CApplication::recordGraphicsCommandBuffer(){}
void CApplication::recordComputeCommandBuffer(){}
void CApplication::postUpdate(){}

void CApplication::UpdateRecordRender(){
    update();

    /**************************
     * 
     * Universial Render Functions
     * 
     * ***********************/
    switch(renderer.m_renderMode){
        case CRenderer::RENDER_GRAPHICS_Mode:
        //case renderer.RENDER_GRAPHICS_Mode:
            //std::cout<<"RENDER_GRAPHICS_Mode"<<std::endl;

            //must wait for fence before record command buffer
            renderer.WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            renderer.AquireSwapchainImage(swapchain); 

            vkResetCommandBuffer(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            recordGraphicsCommandBuffer();

            renderer.SubmitGraphics();

            renderer.PresentSwapchainImage(swapchain); 
        break;
        case CRenderer::RENDER_COMPUTE_Mode:
        //case renderer.RENDER_COMPUTE_Mode:
            //std::cout<<"Application: RENDER_COMPUTE_Mode."<<std::endl;
            renderer.WaitForComputeFence();//must wait for fence before record
            //std::cout<<"Application: renderer.WaitForComputeFence()"<<std::endl;

            vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
            //std::cout<<"Application: vkResetCommandBuffer"<<std::endl;

            recordComputeCommandBuffer();
            //std::cout<<"Application: recordComputeCommandBuffer()"<<std::endl;

            renderer.SubmitCompute();
            //std::cout<<"Application: renderer.SubmitCompute()"<<std::endl;

           // renderer.PresentSwapchainImage(swapchain); //???
        break;
        case CRenderer::RENDER_COMPUTE_SWAPCHAIN_Mode:
        //case renderer.RENDER_COMPUTE_SWAPCHAIN_Mode:
            //must wait for fence before record
            renderer.WaitForComputeFence();
            //must aquire swap image before record command buffer
            renderer.AquireSwapchainImage(swapchain);
            //std::cout<<"Application: renderer.imageIndex = "<<renderer.imageIndex<< std::endl;
            //std::cout<<"Application: renderer.currentFrame = "<<renderer.currentFrame<< std::endl;

            //vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            recordComputeCommandBuffer();

            renderer.SubmitCompute(); 

            renderer.PresentSwapchainImage(swapchain); 
        break;
        case CRenderer::RENDER_COMPUTE_GRAPHICS_Mode:
        //case renderer.RENDER_COMPUTE_GRAPHICS_Mode:
            renderer.WaitForComputeFence();//must wait for fence before record
            renderer.WaitForGraphicsFence();//must wait for fence before record
            renderer.AquireSwapchainImage(swapchain);//must aquire swap image before record command buffer

            vkResetCommandBuffer(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
            vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
            

            recordComputeCommandBuffer();
            recordGraphicsCommandBuffer();
            
            renderer.SubmitCompute(); 
            renderer.SubmitGraphics(); 

            renderer.PresentSwapchainImage(swapchain); 
        break;
        default:
        break;
    }

    //renderer.RecordCompute();
    //recordComputeCommandBuffer();
    //renderer.RecordGraphics();
    //recordGraphicsCommandBuffer();

    //renderer.AquireSwapchainImage(swapchain);
    //renderer.SubmitCompute();
    //renderer.SubmitGraphics();
    //renderer.PresentSwapchainImage(swapchain);     

    //if(renderProcess.bCreateGraphicsPipeline){
        //renderer.preRecordGraphicsCommandBuffer(swapchain);
        //recordGraphicsCommandBuffer();
        //renderer.postRecordGraphicsCommandBuffer(swapchain);
    //}

    //if(renderProcess.bCreateComputePipeline){
        //recordComputeCommandBuffer();
        //renderer.preRecordComputeCommandBuffer(swapchain);
        //renderer.postRecordComputeCommandBuffer(swapchain);
   //}

    postUpdate();

    renderer.Update(); //update currentFrame    
}


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
    for(int i = 0; i < descriptors.size(); i++)
        descriptors[i].DestroyAndFree();
    textureDescriptor.DestroyAndFree();
    for(int i = 0; i < textureImages.size(); i++)
        textureImages[i].Destroy();
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

