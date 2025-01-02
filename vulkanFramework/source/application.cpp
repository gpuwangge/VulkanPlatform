#include "../include/application.h"

//static class members must be defined outside. 
//otherwise invoke 'undefined reference' error when linking
Camera CApplication::mainCamera;
bool CApplication::NeedToExit = false; 
bool CApplication::NeedToPause = false;
std::vector<CObject> CApplication::objectList; 

CApplication::CApplication(){
    //debugger = new CDebugger("../logs/application.log");

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

//void CApplication::initialize(){}//for sample to call
void CApplication::initialize(){
    //Open yaml input file
    m_sampleName.erase(0, 1);
    std::cout<<m_sampleName<<std::endl;
    std::string fullYamlName = "../samples/yaml/" + m_sampleName + ".yaml";
    YAML::Node config;
    try{
        config = YAML::LoadFile(fullYamlName);
    } catch (...){
        std::cout<<"Error loading yaml file"<<std::endl;
        return;
    }

    //Log yaml data for debug
    PRINT("Object::Models Size:  %d", (int)config["Object"]["Models"].size());
    PRINT("Object::Textures Size:  %d", (int)config["Object"]["Textures"].size());
    PRINT("Object::TextureMiplevels Size:  %d", (int)config["Object"]["TextureMiplevels"].size());
    PRINT("Object::TextureSamplers Size:  %d", (int)config["Object"]["TextureSamplers"].size());
    PRINT("Object::TextureCubemap Size:  %d", (int)config["Object"]["TextureCubemap"].size());
    PRINT("Object::VertexShaders Size:  %d", (int)config["Object"]["VertexShaders"].size());
    PRINT("Object::FragmentShaders Size:  %d", (int)config["Object"]["FragmentShaders"].size());
    PRINT("Object::ComputeShaders Size:  %d", (int)config["Object"]["ComputeShaders"].size());
    PRINT("Object::ModelList Size:  %d", (int)config["Object"]["ModelList"].size());
    PRINT("Object::TextureList Size:  %d", (int)config["Object"]["TextureList"].size());
    PRINT("Object::GraphicsPipelineList Size:  %d\n", (int)config["Object"]["GraphicsPipelineList"].size());

    PRINT("Uniform::Graphics Size:  %d", (int)config["Uniform"]["Graphics"].size());
    PRINT("Uniform::Compute Size:  %d", (int)config["Uniform"]["Compute"].size());
    PRINT("Uniform::SamplerAttributes Size:  %d\n", (int)config["Uniform"]["SamplerAttributes"].size());

    PRINT("Feature::GraphicsDepthTest:  %s", config["Feature"]["GraphicsDepthTest"].as<bool>() ? "true":"false");
    PRINT("Feature::GraphicsMSAA:  %s", config["Feature"]["GraphicsMSAA"].as<bool>() ? "true":"false");
    PRINT("Feature::Graphics48BPT:  %s", config["Feature"]["Graphics48BPT"].as<bool>() ? "true":"false");
    PRINT("Feature::GraphicsPushConstant:  %s", config["Feature"]["GraphicsPushConstant"].as<bool>() ? "true":"false");
    PRINT("Feature::GraphicsBlend:  %s", config["Feature"]["GraphicsBlend"].as<bool>() ? "true":"false");
    PRINT("Feature::GraphicsRainbowMipmap:  %s", config["Feature"]["GraphicsRainbowMipmap"].as<bool>() ? "true":"false");
    PRINT("Feature::GraphicsPipelineSkyboxID:  %d\n", config["Feature"]["GraphicsPipelineSkyboxID"].as<int>());

    PRINT("MainCamera::FreeMode:  %s", config["MainCamera"]["FreeMode"].as<bool>() ? "true":"false");
    PRINT("MainCamera::Position Size:  %d", (int)config["MainCamera"]["Position"].size());
    PRINT("MainCamera::Rotation Size:  %d", (int)config["MainCamera"]["Rotation"].size());
    PRINT("MainCamera::TargetLocation Size:  %d", (int)config["MainCamera"]["TargetLocation"].size());
    PRINT("MainCamera::FOV:  %f", config["MainCamera"]["FOV"].as<float>());
    PRINT("MainCamera::Z Size:  %d\n", (int)config["MainCamera"]["Z"].size());

    //Hanlde model yaml data
    if(config["Object"]["Models"].size() > 0) appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(config["Object"]["Models"].as<std::vector<std::string>>()); //std::vector<std::string> {config["Object"]["Models"][0].as<std::string>()}
	else appInfo.Object.Model.Names = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
    
    std::vector<TextureAttributeInfo> textureAttributes = {};
    for(int i = 0; i < config["Object"]["Textures"].size(); i++){
        TextureAttributeInfo info;
        info.name = config["Object"]["Textures"][i].as<std::string>();
        info.miplevel = config["Object"]["TextureMiplevels"][i].as<int>();
        info.samplerid = config["Object"]["TextureSamplers"][i].as<int>();
        info.enableCubemap = config["Object"]["TextureCubemap"][i].as<bool>();
        textureAttributes.push_back(info);
    }
    if(config["Object"]["Textures"].size() > 0) appInfo.Object.Texture.Attributes = std::make_unique<std::vector<TextureAttributeInfo>>(textureAttributes);
    else appInfo.Object.Texture.Attributes = std::make_unique<std::vector<TextureAttributeInfo>>(std::vector<TextureAttributeInfo>());

    if(config["Object"]["VertexShaders"].size() > 0) appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(config["Object"]["VertexShaders"].as<std::vector<std::string>>());// {config["Object"]["VertexShaders"][0].as<std::string>()}
	else appInfo.Object.Pipeline.VertexShader = std::make_unique<std::vector<std::string>>(std::vector<std::string>());

    if(config["Object"]["FragmentShaders"].size() > 0) appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(config["Object"]["FragmentShaders"].as<std::vector<std::string>>()); //std::vector<std::string> {config["Object"]["FragmentShaders"][0].as<std::string>()}
    else appInfo.Object.Pipeline.FragmentShader = std::make_unique<std::vector<std::string>>(std::vector<std::string>());

    if(config["Object"]["ComputeShaders"].size() > 0) appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(config["Object"]["ComputeShaders"].as<std::vector<std::string>>());
    else appInfo.Object.Pipeline.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string>());

    if(config["Object"]["ModelList"].size() > 0) appInfo.Object.Model.List = std::make_unique<std::vector<int>>(config["Object"]["ModelList"].as<std::vector<int>>());
    else appInfo.Object.Model.List = std::make_unique<std::vector<int>>(std::vector<int>());

	if(config["Object"]["TextureList"].size() > 0) appInfo.Object.Texture.List = std::make_unique<std::vector<std::vector<int>>>(config["Object"]["TextureList"].as<std::vector<std::vector<int>>>());
    else appInfo.Object.Texture.List = std::make_unique<std::vector<std::vector<int>>>(std::vector<std::vector<int>>());

	if(config["Object"]["GraphicsPipelineList"].size() > 0) appInfo.Object.Pipeline.GraphicsList = std::make_unique<std::vector<int>>(config["Object"]["GraphicsPipelineList"].as<std::vector<int>>());
    else appInfo.Object.Pipeline.GraphicsList = std::make_unique<std::vector<int>>(std::vector<int>());

    //Handle uniform yaml data
    appInfo.Uniform.GraphicsVector = config["Uniform"]["Graphics"].as<std::vector<std::vector<bool>>>();
    appInfo.Uniform.ComputeVector = config["Uniform"]["Compute"].as<std::vector<std::vector<bool>>>();
    appInfo.Uniform.SamplerAttributes = config["Uniform"]["SamplerAttributes"].as<std::vector<int>>();
    //appInfo.Uniform.GraphicsPipelineSamplers = config["Uniform"]["GraphicsPipelineSamplers"].as<std::vector<int>>();
    //appInfo.Uniform.GraphicsPipelineTextures = config["Uniform"]["GraphicsPipelineTextures"].as<std::vector<int>>();

    //Handle feature yaml data
    appInfo.Feature.EnableGraphicsDepthTest = config["Feature"]["GraphicsDepthTest"].as<bool>();
    appInfo.Feature.EnableGraphicsMSAA = config["Feature"]["GraphicsMSAA"].as<bool>();
    appInfo.Feature.EnableGraphics48BPT = config["Feature"]["Graphics48BPT"].as<bool>();
    appInfo.Feature.EnableGraphicsPushConstant = config["Feature"]["GraphicsPushConstant"].as<bool>();
    appInfo.Feature.EnableGraphicsBlend = config["Feature"]["GraphicsBlend"].as<bool>();
    appInfo.Feature.EnableGraphicsRainbowMipmap = config["Feature"]["GraphicsRainbowMipmap"].as<bool>();
    //appInfo.Feature.EnableGraphicsCubemap = config["Feature"]["GraphicsCubemap"].as<bool>();
    appInfo.Feature.GraphicsPipelineSkyboxID = config["Feature"]["GraphicsPipelineSkyboxID"].as<int>();
   
    //Handle camera yaml data
    if(config["MainCamera"]["FreeMode"].as<bool>() == true) mainCamera.cameraType = Camera::CameraType::freemove;
    mainCamera.SetTargetPosition(config["MainCamera"]["TargetLocation"][0].as<float>(), config["MainCamera"]["TargetLocation"][1].as<float>(), config["MainCamera"]["TargetLocation"][2].as<float>());
    mainCamera.SetPosition(config["MainCamera"]["Position"][0].as<float>(), config["MainCamera"]["Position"][1].as<float>(), config["MainCamera"]["Position"][2].as<float>());
    mainCamera.SetRotation(config["MainCamera"]["Rotation"][0].as<float>(), config["MainCamera"]["Rotation"][1].as<float>(), config["MainCamera"]["Rotation"][2].as<float>());
    mainCamera.setPerspective(config["MainCamera"]["FOV"].as<float>(),  (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
        config["MainCamera"]["Z"][0].as<float>(), config["MainCamera"]["Z"][1].as<float>());

    
    //Real Initialization Starts Here
    objectList.resize(appInfo.Object.Pipeline.GraphicsList->size()); //each object should have a pipeline reference, so use the pipeline size as object size. must set this before Set App Property(because of descriptor size rely on object size)
    
    //auto startAppTime = std::chrono::high_resolution_clock::now();
    SetApplicationProperty(appInfo);
    //auto endAppTime = std::chrono::high_resolution_clock::now();
    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endAppTime - startAppTime).count() * 1000;
    //std::cout<<"Total Set Application Property cost: "<<durationTime<<" milliseconds"<<std::endl;

    std::vector<int> texture_ids; 
    int model_id = -1;
    int graphics_pipeline_id = -1;
    for(int i = 0; i < objectList.size(); i++){
        if(appInfo.Object.Texture.List->size() > 0) texture_ids = (*appInfo.Object.Texture.List)[i]; 
        if(appInfo.Object.Model.List->size() > 0) model_id = (*appInfo.Object.Model.List)[i];
        if(appInfo.Object.Pipeline.GraphicsList->size() > 0) graphics_pipeline_id = (*appInfo.Object.Pipeline.GraphicsList)[i];

        if(CSupervisor::VertexStructureType == VertexStructureTypes::TwoDimension || CSupervisor::VertexStructureType  == VertexStructureTypes::ThreeDimension)
            objectList[i].Register((CApplication*)this, texture_ids, model_id, i, graphics_pipeline_id, modelManager.modelLengths[model_id], modelManager.modelLengthsMin[model_id], modelManager.modelLengthsMax[model_id]); //must be set after initialize()::SetApplicationProperty(appInfo);
        else
            objectList[i].Register((CApplication*)this, texture_ids, model_id, i, graphics_pipeline_id, glm::vec3(), glm::vec3(), glm::vec3());
        if(graphics_pipeline_id == appInfo.Feature.GraphicsPipelineSkyboxID)  objectList[i].bSkybox = true;
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

    //CSupervisor::ComputeShader = appInfo.Shader.Compute;
	//CSupervisor::VertexShader = appInfo.Shader.Vertex;
	//CSupervisor::FragmentShader = appInfo.Shader.Fragment;

    //Uniforms
    int i = 0; //assume all graphics pipelines use the same descriptor uniform layout/set
    if(appInfo.Uniform.GraphicsVector.size() > 0){
        //if(appInfo.Uniform.GraphicsCustom.Size) CSupervisor::Activate_Uniform_Graphics_Custom(appInfo.Uniform.GraphicsCustom.Size, appInfo.Uniform.GraphicsCustom.Binding);
        if(appInfo.Uniform.GraphicsVector[i][0]) CSupervisor::Activate_Uniform_Graphics_Custom(appInfo.Uniform.GraphicsCustom.Size, appInfo.Uniform.GraphicsCustom.Binding);
        if(appInfo.Uniform.GraphicsVector[i][1]) CSupervisor::Activate_Uniform_Graphics_MVP();
        if(appInfo.Uniform.GraphicsVector[i][2]) CSupervisor::Activate_Uniform_Graphics_VP();
    }
    //if(appInfo.Uniform.SamplerVector.size() > 0) CSupervisor::Activate_Uniform_Graphics_Sampler(appInfo.Uniform.SamplerVector[i]); //samplerCount
    if(appInfo.Uniform.SamplerAttributes.size() > 0) CSupervisor::Activate_Uniform_Graphics_Sampler(); //samplerCount
    
    int j = 0; //assume all compute pipelines use the same descriptor uniform layout/set
    if(appInfo.Uniform.ComputeVector.size() > 0){ //assume sampler has exactly one compute pipeline, so the first index is 0
        //if(appInfo.Uniform.ComputeCustom.Size) CSupervisor::Activate_Uniform_Compute_Custom(appInfo.Uniform.ComputeCustom.Size, appInfo.Uniform.ComputeCustom.Binding);
        //if(appInfo.Uniform.ComputeStorageBuffer.Size) CSupervisor::Activate_Uniform_Compute_StorageBuffer(appInfo.Uniform.ComputeStorageBuffer.Size, appInfo.Uniform.ComputeStorageBuffer.Usage);
        if(appInfo.Uniform.ComputeVector[j][0]) CSupervisor::Activate_Uniform_Compute_Custom(appInfo.Uniform.ComputeCustom.Size, appInfo.Uniform.ComputeCustom.Binding);
        if(appInfo.Uniform.ComputeVector[j][1]) CSupervisor::Activate_Uniform_Compute_StorageBuffer(appInfo.Uniform.ComputeStorageBuffer.Size, appInfo.Uniform.ComputeStorageBuffer.Usage);
        if(appInfo.Uniform.ComputeVector[j][2]) CSupervisor::Activate_Uniform_Compute_StorageImage_Swapchain();//EnableComputeStorageImageSwapChain
        if(appInfo.Uniform.ComputeVector[j][3]) CSupervisor::Activate_Uniform_Compute_StorageImage(); //EnableComputeStorageImage
    }

    if(appInfo.Feature.EnableGraphicsBlend) CSupervisor::Activate_Feature_Graphics_Blend();
    if(appInfo.Feature.EnableGraphicsDepthTest) CSupervisor::Activate_Feature_Graphics_DepthTest();
    if(appInfo.Feature.EnableGraphicsMSAA) CSupervisor::Activate_Feature_Graphics_MSAA();
    if(appInfo.Feature.EnableGraphics48BPT) CSupervisor::Activate_Feature_Graphics_48BPT();
    if(appInfo.Feature.EnableGraphicsPushConstant) CSupervisor::Activate_Feature_Graphics_PushConstant();
    if(appInfo.Feature.EnableGraphicsRainbowMipmap) CSupervisor::Activate_Feature_Graphics_RainbowMipmap();
    if(appInfo.Feature.GraphicsPipelineSkyboxID != -1) {
        //objectList[appInfo.Feature.GraphicsPipelineSkyboxID].bSkybox = true;
        renderProcess.skyboxID = appInfo.Feature.GraphicsPipelineSkyboxID;
    }
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
    if(appInfo.Object.Texture.Attributes != NULL)
        CSupervisor::Activate_Texture(std::move(appInfo.Object.Texture.Attributes));
    auto endTextureTime = std::chrono::high_resolution_clock::now();
    auto durationTextureTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
    std::cout<<"Load Textures cost: "<<durationTextureTime<<" milliseconds"<<std::endl;

    //auto startPipelineTime = std::chrono::high_resolution_clock::now();
    CSupervisor::Activate_Pipeline();
    //auto endPipelineTime = std::chrono::high_resolution_clock::now();
    //durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endPipelineTime - startPipelineTime).count()*1000;
    //std::cout<<"Activate Pipeline cost: "<<durationTime<<" milliseconds"<<std::endl;
    
}
