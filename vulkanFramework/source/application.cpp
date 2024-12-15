#include "../include/application.h"

//static class members must be defined outside. 
//otherwise invoke 'undefined reference' error when linking
Camera CApplication::mainCamera;
bool CApplication::NeedToExit = false; 
bool CApplication::NeedToPause = false;
std::vector<CObject> CApplication::objectList; 

CApplication::CApplication(){
    //debugger = new CDebugger("../logs/application.log");

    
    mainCamera.cameraType = Camera::CameraType::lookat; //default
    mainCamera.SetTargetPosition(0,0,0);
    mainCamera.SetPosition(0.0f, 0.0f, -3.0f);
    //mainCamera.SetRotation(0.0f, 0.0f, 0.0f);
    mainCamera.setPerspective(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    //mainCamera.movementSpeed = 1.0f;
    //mainCamera.rotationSpeed = 200.0f;

    //NeedToExit = false;
    windowWidth = 0;
    windowHeight = 0;
}

#ifndef ANDROID
void CApplication::run(){ //Entrance Function
    CContext::Init();

    /**************** 
    * Five steps with third-party(GLFW or SDL) initialization
    * Step 1: Create Window
    *****************/
#ifdef SDL
    sdlManager.createWindow(OUT windowWidth, OUT windowHeight);
#else
    glfwManager.createWindow(OUT windowWidth, OUT windowHeight);
#endif
	PRINT("run: Created Window. Window width = %d,  height = %d.", windowWidth, windowHeight);

    /**************** 
    * Step 2: Select required layers
    *****************/
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    
    /**************** 
    * Step 3: Select required instance extensions
    *****************/
    std::vector<const char*> requiredInstanceExtensions;
#ifdef SDL
    sdlManager.queryRequiredInstanceExtensions(OUT requiredInstanceExtensions);
#else    
    glfwManager.queryRequiredInstanceExtensions(OUT requiredInstanceExtensions);
#endif
    if(enableValidationLayers) requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    /**************** 
    * Step 4: create instance
    *****************/
    //prepareVulkanDevices();
    instance = std::make_unique<CInstance>(requiredValidationLayers, requiredInstanceExtensions);

    /**************** 
    * Step 5: create surface
    * Surface is to store view format information for creating swapchain. 
    * Only third party(glfw or sdl) knows what kind of surface can be attached to its window.
    *****************/
#ifdef SDL   
    sdlManager.createSurface(IN instance, OUT surface);
#else  
    glfwManager.createSurface(IN instance, OUT surface);
#endif

    /**************** 
    * General initialization begins
    * Select required queue families
    * Select required device extensions
    *****************/
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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

    CSupervisor::Register((CApplication*)this);

    std::cout<<"======================================="<<std::endl;
    std::cout<<"======Welcome to Vulkan Platform======="<<std::endl;
    std::cout<<"======================================="<<std::endl;

    auto startInitialzeTime = std::chrono::high_resolution_clock::now();
    initialize();
    auto endInitializeTime = std::chrono::high_resolution_clock::now();
    auto durationInitializationTime = std::chrono::duration<float, std::chrono::seconds::period>(endInitializeTime - startInitialzeTime).count() * 1000;
    std::cout<<"Total Initialization cost: "<<durationInitializationTime<<" milliseconds"<<std::endl;

#ifdef SDL   
    while(sdlManager.bStillRunning) {
        sdlManager.eventHandle();
        if(!NeedToPause) UpdateRecordRender();
        if(NeedToExit) break;
    }
#else  
    while (!glfwWindowShouldClose(glfwManager.window)) {
        glfwPollEvents();
        if(!NeedToPause) UpdateRecordRender();
        if(NeedToExit) break;
	}
#endif

	vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
}
#endif

void CApplication::initialize(){
    //auto startAppTime = std::chrono::high_resolution_clock::now();
    SetApplicationProperty(appInfo);
    //auto endAppTime = std::chrono::high_resolution_clock::now();
    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endAppTime - startAppTime).count() * 1000;
    //std::cout<<"Total Set Application Property cost: "<<durationTime<<" milliseconds"<<std::endl;

    objectList.resize(appInfo.Object.Count);
    int texture_id = -1; //INT_MAX means use all samplers
    int model_id = -1;
    for(int i = 0; i < objectList.size(); i++){
        if(appInfo.Object.Texture.List != NULL) texture_id = appInfo.Uniform.GraphicsSampler.UseMultiSamplerForOneObject ? INT_MAX : (*appInfo.Object.Texture.List)[i]; 
        //std::cout<<"i: "<<(*appInfo.Object.Model.List)[i] <<std::endl;
        if(appInfo.Object.Model.List != NULL) model_id = (*appInfo.Object.Model.List)[i];
        if(CSupervisor::VertexStructureType == VertexStructureTypes::TwoDimension || CSupervisor::VertexStructureType  == VertexStructureTypes::ThreeDimension)
            objectList[i].Register((CApplication*)this, texture_id, model_id, i, 0, modelManager.modelLengths[model_id], modelManager.modelLengthsMin[model_id], modelManager.modelLengthsMax[model_id]); //must be set after initialize()::SetApplicationProperty(appInfo);
        else
            objectList[i].Register((CApplication*)this, texture_id, model_id, i, 0, glm::vec3(), glm::vec3(), glm::vec3());
    }

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

    for(int i = 0; i < objectList.size(); i++) objectList[i].Update(deltaTime);

    graphicsDescriptorManager.updateMVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera, objectList);
    graphicsDescriptorManager.updateVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera);
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

            renderer.StartRecordGraphicsCommandBuffer(
                renderProcess.renderPass, 
                swapchain.swapChainFramebuffers,swapchain.swapChainExtent, 
                renderProcess.clearValues);
            recordGraphicsCommandBuffer();
            renderer.EndRecordGraphicsCommandBuffer();

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

            renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            recordComputeCommandBuffer();
            renderer.EndRecordComputeCommandBuffer();
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

            //in this mode, nothing is recorded(all commands are pre-recorded), for NOW. But still, swapchain will be presented.
            //renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            //recordComputeCommandBuffer();
            //renderer.EndRecordComputeCommandBuffer();

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
            
            renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            recordComputeCommandBuffer();
            renderer.EndRecordComputeCommandBuffer();

            renderer.StartRecordGraphicsCommandBuffer(
                renderProcess.renderPass, 
                swapchain.swapChainFramebuffers,swapchain.swapChainExtent, 
                renderProcess.clearValues);
            recordGraphicsCommandBuffer();
            renderer.EndRecordGraphicsCommandBuffer();
            
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
   //for(int i = 0; i < descriptors.size(); i++)
    //    descriptors[i].DestroyAndFree();
    graphicsDescriptorManager.DestroyAndFree();
    computeDescriptorManager.DestroyAndFree();
    //textureDescriptor.DestroyAndFree();
    //for(int i = 0; i < textureImages.size(); i++) textureImages[i].Destroy();
    //for(int i = 0; i < textureImages1.size(); i++) textureImages1[i].Destroy();
    //for(int i = 0; i < textureImages2.size(); i++) textureImages2[i].Destroy();
    textureManager.Destroy();
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

/*************
 * Helper Functions
 *******/
void CApplication::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    CSupervisor::Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
}

void CApplication::SetApplicationProperty(AppInfo &appInfo){
    //auto startActivateTime = std::chrono::high_resolution_clock::now();

    renderer.m_renderMode = appInfo.Render.Mode;

    CSupervisor::ComputeShader = appInfo.Shader.Compute;
	CSupervisor::VertexShader = appInfo.Shader.Vertex;
	CSupervisor::FragmentShader = appInfo.Shader.Fragment;

    if(appInfo.Uniform.GraphicsSampler.Count) CSupervisor::Activate_Uniform_Graphics_Sampler(appInfo.Uniform.GraphicsSampler.Count); //samplerCount
    if(appInfo.Uniform.EnableGraphicsMVP) CSupervisor::Activate_Uniform_Graphics_MVP();
    if(appInfo.Uniform.EnableGraphicsVP) CSupervisor::Activate_Uniform_Graphics_VP();
    if(appInfo.Uniform.GraphicsCustom.Size) CSupervisor::Activate_Uniform_Graphics_Custom(appInfo.Uniform.GraphicsCustom.Size, appInfo.Uniform.GraphicsCustom.Binding);
    if(appInfo.Uniform.ComputeStorageBuffer.Size) CSupervisor::Activate_Uniform_Compute_StorageBuffer(appInfo.Uniform.ComputeStorageBuffer.Size, appInfo.Uniform.ComputeStorageBuffer.Usage);
    if(appInfo.Uniform.ComputeCustom.Size) CSupervisor::Activate_Uniform_Compute_Custom(appInfo.Uniform.ComputeCustom.Size, appInfo.Uniform.ComputeCustom.Binding);
    if(appInfo.Uniform.EnableComputeStorageImage) CSupervisor::Activate_Uniform_Compute_StorageImage();
    if(appInfo.Uniform.EnableComputeStorageImageSwapChain) CSupervisor::Activate_Uniform_Compute_StorageImage_Swapchain();
    if(appInfo.Feature.EnableGraphicsBlend) CSupervisor::Activate_Feature_Graphics_Blend();
    if(appInfo.Feature.EnableGraphicsDepthTest) CSupervisor::Activate_Feature_Graphics_DepthTest();
    if(appInfo.Feature.EnableGraphicsMSAA) CSupervisor::Activate_Feature_Graphics_MSAA();
    if(appInfo.Feature.EnableGraphics48BPT) CSupervisor::Activate_Feature_Graphics_48BPT();
    if(appInfo.Feature.EnableGraphicsPushConstant) CSupervisor::Activate_Feature_Graphics_PushConstant();
    if(appInfo.Feature.EnableGraphicsRainbowMipmap) CSupervisor::Activate_Feature_Graphics_RainbowMipmap();
    if(appInfo.Buffer.GraphicsVertex.StructureType != VertexStructureTypes::NoType) CSupervisor::Activate_Buffer_Graphics_Vertex(appInfo.Buffer.GraphicsVertex.StructureType);
    //auto endActivateTime = std::chrono::high_resolution_clock::now();
    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endActivateTime - startActivateTime).count()*1000;
    //std::cout<<"Activate cost: "<<durationTime<<" milliseconds"<<std::endl;

    auto startLoadModelTime = std::chrono::high_resolution_clock::now();
    if(appInfo.Object.Model.Names != NULL) CSupervisor::Activate_Buffer_Graphics_Vertex(std::move(appInfo.Object.Model.Names), modelManager); 
    auto endLoadModelTime = std::chrono::high_resolution_clock::now();
    auto durationLoadModelTime = std::chrono::duration<float, std::chrono::seconds::period>(endLoadModelTime - startLoadModelTime).count()*1000;
    std::cout<<"Load Model cost: "<<durationLoadModelTime<<" milliseconds"<<std::endl;

    auto startTextureTime = std::chrono::high_resolution_clock::now();
    if(appInfo.Object.Texture.Names != NULL)
        CSupervisor::Activate_Texture(std::move(appInfo.Object.Texture.Names), appInfo.Feature.EnableCubemap);
    auto endTextureTime = std::chrono::high_resolution_clock::now();
    auto durationTextureTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
    std::cout<<"Load Textures cost: "<<durationTextureTime<<" milliseconds"<<std::endl;

    //auto startPipelineTime = std::chrono::high_resolution_clock::now();
    CSupervisor::Activate_Pipeline();
    //auto endPipelineTime = std::chrono::high_resolution_clock::now();
    //durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endPipelineTime - startPipelineTime).count()*1000;
    //std::cout<<"Activate Pipeline cost: "<<durationTime<<" milliseconds"<<std::endl;

}
