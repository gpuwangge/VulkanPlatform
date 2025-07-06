#include "../include/application.h"

//static class members must be defined outside. 
//otherwise invoke 'undefined reference' error when linking
Camera CApplication::mainCamera;
Camera CApplication::lightCamera;
bool CApplication::NeedToExit = false; 
bool CApplication::NeedToPause = false;
int CApplication::focusObjectId = 0;
std::vector<CObject> CApplication::objects; 
std::vector<CLight> CApplication::lights; 

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
    m_sampleName.erase(0, 1);
#ifdef SDL
    sdlManager.createWindow(OUT windowWidth, OUT windowHeight, m_sampleName);
#else
    glfwManager.createWindow(OUT windowWidth, OUT windowHeight, m_sampleName);
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

    renderer.CreateCommandPool(surface);

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
    glfwSwapInterval(1); //set V-Sync. wait a refresh period, normally 16.67ms
    while (!glfwWindowShouldClose(glfwManager.window)) {
        glfwPollEvents();
        if(!NeedToPause) UpdateRecordRender();
        if(NeedToExit) break;

        if(mainCamera.cameraType == Camera::CameraType::FREE){
            mainCamera.AngularVelocity.x = 0;
            mainCamera.AngularVelocity.y = 0;
        }

        glfwWaitEventsTimeout(1.0 / 60.0);
    }
#endif

	vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
}
#endif

void CApplication::initialize(){
    std::string fullYamlName = "../samples/yaml/" + m_sampleName + ".yaml";
    try{
        config = YAML::LoadFile(fullYamlName);
    } catch (...){
        std::cout<<"Error loading yaml file"<<std::endl;
        return;
    }

    /****************************
    * 1 Initialize ObjectList and LightList
    ****************************/
    if (config["Objects"]) {
        int max_object_id = 0;
        for (const auto& obj : config["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            max_object_id = (object_id > max_object_id) ? object_id : max_object_id;
        }
        objects.resize(((max_object_id+1) < config["Objects"].size())?(max_object_id+1):config["Objects"].size()); 
        std::cout<<"Object Size: "<<objects.size()<<std::endl;
    }
    if (config["Lights"]) {
        int max_light_d = 0;
        for (const auto& light : config["Lights"]) {
            int light_id = light["light_id"] ? light["light_id"].as<int>() : 0;
            max_light_d = (light_id > max_light_d) ? light_id : max_light_d;
        }
        lights.resize(((max_light_d+1) < config["Lights"].size())?(max_light_d+1):config["Lights"].size()); 
        std::cout<<"Light Size: "<<lights.size()<<std::endl;
    }

    /****************************
    * 2 Read Features
    ****************************/   
    renderer.m_renderMode = appInfo.RenderMode;
    ReadFeatures();

    /****************************
    * 3 Read Uniforms
    ****************************/
    ReadUniforms();

    /****************************
    * 3.2 Read Subpasses
    ****************************/
    ReadAttachments();

    /****************************
    * 3.5 Read Subpasses
    ****************************/
    ReadSubpasses();
    
    /****************************
    * 4 Read Resources
    ****************************/
    //When creating texture resource, need uniform information, so must read uniforms before read resources
    ReadResources();

    /****************************
    * 5 Create Uniform Descriptors
    ****************************/
    bool b_uniform_graphics = appInfo.Uniform.b_uniform_graphics_custom || appInfo.Uniform.b_uniform_graphics_mvp || appInfo.Uniform.b_uniform_graphics_vp;
    bool b_uniform_compute = appInfo.Uniform.b_uniform_compute_custom || appInfo.Uniform.b_uniform_compute_storage || appInfo.Uniform.b_uniform_compute_swapchain_storage || appInfo.Uniform.b_uniform_compute_texture_storage;
    CreateUniformDescriptors(b_uniform_graphics, b_uniform_compute);

    /****************************
    * 6 Create Pipelines
    ****************************/
    CreatePipelines();

    /****************************
    * 7 Read and Register Objects
    ****************************/
    ReadRegisterObjects();

    /****************************
    * 8 Read Lightings
    ****************************/
    ReadLightings();
    
    /****************************
    * 9 Read Main Camera
    ****************************/
    ReadMainCamera();

    /****************************
    * 10 Create Sync Objects and Clean up Shaders
    ****************************/
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
    static auto startTime = std::chrono::high_resolution_clock::now();
    static auto lastTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    durationTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = currentTime;

    if(objects.size() > 0 && focusObjectId < objects.size()){
        mainCamera.SetTargetPosition(objects[focusObjectId].Position);
        lightCamera.SetTargetPosition(objects[focusObjectId].Position);
    }
    mainCamera.update(deltaTime);
    lightCamera.update(deltaTime);
    for(int i = 0; i < objects.size(); i++) objects[i].Update(deltaTime, renderer.currentFrame, mainCamera, lightCamera); 
    for(int i = 0; i < lights.size(); i++) lights[i].Update(deltaTime, renderer.currentFrame, mainCamera); 
    
}

void CApplication::recordGraphicsCommandBuffer_renderpassMainscene(){}
void CApplication::recordGraphicsCommandBuffer_renderpassShadowmap(){}
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
        case CRenderer::GRAPHICS:
        //case renderer.RENDER_GRAPHICS_Mode:
            //std::cout<<"RENDER_GRAPHICS_Mode"<<std::endl;

            //must wait for fence before record command buffer
            renderer.WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            renderer.AquireSwapchainImage(swapchain); 

            vkResetCommandBuffer(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            renderer.StartRecordGraphicsCommandBuffer(
                renderProcess.renderPass_mainscene, 
                swapchain.framebuffers_mainscene,swapchain.swapChainExtent, 
                renderProcess.clearValues);
            recordGraphicsCommandBuffer_renderpassMainscene();
            renderer.EndRecordGraphicsCommandBuffer();

            renderer.SubmitGraphics();

            renderer.PresentSwapchainImage(swapchain);
        break;
        case CRenderer::GRAPHICS_SHADOWMAP:
            //must wait for fence before record command buffer
            renderer.WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            renderer.AquireSwapchainImage(swapchain); 

            vkResetCommandBuffer(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            renderer.BeginCommandBuffer(renderer.graphicsCmdId);


            //std::cout<<"Application: Begin Shadowmap Render Pass."<<std::endl;
            renderer.BeginRenderPass(renderProcess.renderPass_shadowmap, swapchain.framebuffers_shadowmap, swapchain.swapChainExtent, renderProcess.clearValues_shadowmap, true);
            renderer.SetViewport(swapchain.swapChainExtent);
            renderer.SetScissor(swapchain.swapChainExtent);
            recordGraphicsCommandBuffer_renderpassShadowmap();
            renderer.EndRenderPass();

            //std::cout<<"Application: Begin Mainscene Render Pass."<<std::endl;
            renderer.BeginRenderPass(renderProcess.renderPass_mainscene, swapchain.framebuffers_mainscene, swapchain.swapChainExtent, renderProcess.clearValues, false);
            renderer.SetViewport(swapchain.swapChainExtent);
            renderer.SetScissor(swapchain.swapChainExtent);
            recordGraphicsCommandBuffer_renderpassMainscene();
            renderer.EndRenderPass();

	        renderer.EndCommandBuffer(renderer.graphicsCmdId);

            renderer.SubmitGraphics();

            renderer.PresentSwapchainImage(swapchain);


        break;
        case CRenderer::COMPUTE:
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
        case CRenderer::COMPUTE_SWAPCHAIN:
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
        case CRenderer::COMPUTE_GRAPHICS:
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
                renderProcess.renderPass_mainscene,
                swapchain.framebuffers_mainscene, swapchain.swapChainExtent,
                renderProcess.clearValues);
            recordGraphicsCommandBuffer_renderpassMainscene();
            renderer.EndRecordGraphicsCommandBuffer();
            
            renderer.SubmitCompute(); 
            renderer.SubmitGraphics(); 

            renderer.PresentSwapchainImage(swapchain); 
        break;
        default:
        break;
    }

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
    //std::cout<<"Application: swapchain.CleanUp()"<<std::endl;
    swapchain.CleanUp();
    //std::cout<<"Application: renderProcess.CleanUp()"<<std::endl;
    renderProcess.Cleanup();

    //std::cout<<"Application: graphicsDescriptorManager.Destroy()"<<std::endl;
    graphicsDescriptorManager.DestroyAndFree();
    //std::cout<<"Application: computeDescriptorManager.DestroyAndFree()"<<std::endl;
    computeDescriptorManager.DestroyAndFree();

    //std::cout<<"Application: textureManager.Destroy()"<<std::endl;
    textureManager.Destroy();
    //std::cout<<"Application: renderer.Destroy()"<<std::endl;
    renderer.Destroy();

    //std::cout<<"Application: vkDestroyDevice()"<<std::endl;
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

void CApplication::ReadFeatures(){
    //appInfo.Feature.b_feature_graphics_depth_test = config["Features"]["feature_graphics_depth_test"] ? config["Features"]["feature_graphics_depth_test"].as<bool>() : false;
    //appInfo.Feature.b_feature_graphics_msaa = config["Features"]["feature_graphics_msaa"] ? config["Features"]["feature_graphics_msaa"].as<bool>() : false;
    appInfo.Feature.b_feature_graphics_48pbt = config["Features"]["feature_graphics_48pbt"] ? config["Features"]["feature_graphics_48pbt"].as<bool>() : false;
    appInfo.Feature.b_feature_graphics_push_constant = config["Features"]["feature_graphics_push_constant"] ? config["Features"]["feature_graphics_push_constant"].as<bool>() : false;
    appInfo.Feature.b_feature_graphics_blend = config["Features"]["feature_graphics_blend"] ? config["Features"]["feature_graphics_blend"].as<bool>() : false;
    appInfo.Feature.b_feature_graphics_rainbow_mipmap = config["Features"]["feature_graphics_rainbow_mipmap"] ? config["Features"]["feature_graphics_rainbow_mipmap"].as<bool>() : false;
    appInfo.Feature.feature_graphics_pipeline_skybox_id = config["Features"]["feature_graphics_pipeline_skybox_id"] ? config["Features"]["feature_graphics_pipeline_skybox_id"].as<int>() : -1;
    appInfo.Feature.feature_graphics_observe_attachment_id = config["Features"]["feature_graphics_observe_attachment_id"] ? config["Features"]["feature_graphics_observe_attachment_id"].as<int>() : -1;

    if(appInfo.Feature.b_feature_graphics_push_constant){
        shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);
    }
    if(appInfo.Feature.b_feature_graphics_blend){
        renderProcess.addColorBlendAttachment(
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);        
    }
    renderProcess.skyboxID = appInfo.Feature.feature_graphics_pipeline_skybox_id;
}

void CApplication::ReadUniforms(){
    for (const auto& uniform : config["Uniforms"]) {
        if (uniform["Graphics"]) {
            for (const auto& graphicsUniform : uniform["Graphics"]) {
                std::string name = graphicsUniform["uniform_graphics_name"] ? graphicsUniform["uniform_graphics_name"].as<std::string>() : "Default";
                appInfo.Uniform.b_uniform_graphics_custom = graphicsUniform["uniform_graphics_custom"] ? graphicsUniform["uniform_graphics_custom"].as<bool>() : false;
                appInfo.Uniform.b_uniform_graphics_mvp = graphicsUniform["uniform_graphics_mvp"] ? graphicsUniform["uniform_graphics_mvp"].as<bool>() : false;
                appInfo.Uniform.b_uniform_graphics_vp = graphicsUniform["uniform_graphics_vp"] ? graphicsUniform["uniform_graphics_vp"].as<bool>() : false;
                appInfo.Uniform.b_uniform_graphics_lighting = graphicsUniform["uniform_graphics_lighting"] ? graphicsUniform["uniform_graphics_lighting"].as<bool>() : false;
                appInfo.Uniform.b_uniform_graphics_depth_image_sampler = graphicsUniform["uniform_graphics_depth_image_sampler"] ? graphicsUniform["uniform_graphics_depth_image_sampler"].as<bool>() : false;
                appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler = graphicsUniform["uniform_graphics_lightdepth_image_sampler"] ? graphicsUniform["uniform_graphics_lightdepth_image_sampler"].as<bool>() : false;
                appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler_hardware = graphicsUniform["uniform_graphics_lightdepth_image_sampler_hardware"] ? graphicsUniform["uniform_graphics_lightdepth_image_sampler_hardware"].as<bool>() : false;
  
                if(appInfo.Uniform.b_uniform_graphics_custom)
                    //CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_CUSTOM;
                    CGraphicsDescriptorManager::addCustomUniformBuffer(appInfo.Uniform.GraphicsCustom.Size);

                if(appInfo.Uniform.b_uniform_graphics_lighting)
                    //CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_LIGHTING;
                    CGraphicsDescriptorManager::addLightingUniformBuffer();

                if(appInfo.Uniform.b_uniform_graphics_mvp)
                    //CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_MVP;
                    CGraphicsDescriptorManager::addMVPUniformBuffer();

                if(appInfo.Uniform.b_uniform_graphics_vp)
                    //CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_VP;
                    CGraphicsDescriptorManager::addVPUniformBuffer();

                if(appInfo.Uniform.b_uniform_graphics_depth_image_sampler)
                    CGraphicsDescriptorManager::addDepthImageSamplerUniformBuffer();

                if(appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler)
                    CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer();

                if(appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler_hardware)
                    CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer_hardwareDepthBias();

            }
        }

        if (uniform["Compute"]) {
            for (const auto& computeUniform : uniform["Compute"]) {
                std::string name = computeUniform["uniform_compute_name"] ? computeUniform["uniform_compute_name"].as<std::string>() : "Default";
                appInfo.Uniform.b_uniform_compute_custom = computeUniform["uniform_compute_custom"] ? computeUniform["uniform_compute_custom"].as<bool>() : false;
                appInfo.Uniform.b_uniform_compute_storage = computeUniform["uniform_compute_storage"] ? computeUniform["uniform_compute_storage"].as<bool>() : false;
                appInfo.Uniform.b_uniform_compute_swapchain_storage = computeUniform["uniform_compute_swapchain_storage"] ? computeUniform["uniform_compute_swapchain_storage"].as<bool>() : false;
                appInfo.Uniform.b_uniform_compute_texture_storage = computeUniform["uniform_compute_texture_storage"] ? computeUniform["uniform_compute_texture_storage"].as<bool>() : false;
            
                if(appInfo.Uniform.b_uniform_compute_custom)
                    //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_UNIFORMBUFFER_CUSTOM;
                    CComputeDescriptorManager::addCustomUniformBuffer(appInfo.Uniform.ComputeCustom.Size);

                if(appInfo.Uniform.b_uniform_compute_storage)
                    //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_STORAGEBUFFER_DOUBLE;
                    CComputeDescriptorManager::addStorageBuffer(appInfo.Uniform.ComputeStorageBuffer.Size, appInfo.Uniform.ComputeStorageBuffer.Usage);

                if(appInfo.Uniform.b_uniform_compute_texture_storage)
                    //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_STORAGEIMAGE_TEXTURE;
                    CComputeDescriptorManager::addStorageImage(COMPUTE_STORAGEIMAGE_TEXTURE);

                if(appInfo.Uniform.b_uniform_compute_swapchain_storage)
                    //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_STORAGEIMAGE_SWAPCHAIN;
                    CComputeDescriptorManager::addStorageImage(COMPUTE_STORAGEIMAGE_SWAPCHAIN);
            }  
        }

        if (uniform["GraphicsTextureImageSamplers"]) {
            std::vector<int> miplevels;
            for (const auto& samplerUniform : uniform["GraphicsTextureImageSamplers"]) {
                std::string name = samplerUniform["uniform_graphics_texture_image_sampler_name"] ? samplerUniform["uniform_graphics_texture_image_sampler_name"].as<std::string>() : "Default";
                int miplevel = samplerUniform["uniform_graphics_texture_image_sampler_miplevel"] ? samplerUniform["uniform_graphics_texture_image_sampler_miplevel"].as<int>() : 1;
                miplevels.push_back(miplevel);
            }
            CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE;
            CGraphicsDescriptorManager::addTextureImageSamplerUniformBuffer(miplevels);
        }
    }
}

void CApplication::ReadResources(){
    for (const auto& resource : config["Resources"]) {
        if (resource["Models"]) {
            for (const auto& model : resource["Models"]) {
                std::string name = model["resource_model_name"] ? model["resource_model_name"].as<std::string>() : "Default";
                //std::cout<<"model name: "<<name<<std::endl;
                //id is not really useful here, because the model id must be in order
                int id = model["resource_model_id"] ? model["resource_model_id"].as<int>() : 0;

                appInfo.VertexBufferType = VertexStructureTypes::ThreeDimension;
                if(name == "CUSTOM3D0"){
                    renderer.CreateVertexBuffer<Vertex3D>(modelManager.customModels3D[0].vertices); 
                    renderer.CreateIndexBuffer(modelManager.customModels3D[0].indices);
                    
                    modelManager.modelLengths.push_back(modelManager.customModels3D[0].length);
                    modelManager.modelLengthsMin.push_back(modelManager.customModels3D[0].lengthMin);
                    modelManager.modelLengthsMax.push_back(modelManager.customModels3D[0].lengthMax);
                }else if(name == "CUSTOM2D0"){
                    appInfo.VertexBufferType = VertexStructureTypes::TwoDimension;
                    renderer.CreateVertexBuffer<Vertex2D>(modelManager.customModels2D[0].vertices); 

                    modelManager.modelLengths.push_back(modelManager.customModels2D[0].length);
                    modelManager.modelLengthsMin.push_back(modelManager.customModels2D[0].lengthMin);
                    modelManager.modelLengthsMax.push_back(modelManager.customModels2D[0].lengthMax);
                }else{
                    appInfo.VertexBufferType = VertexStructureTypes::ThreeDimension;
                    std::vector<Vertex3D> modelVertices3D;
                    std::vector<uint32_t> modelIndices3D;
                    modelManager.LoadObjModel(name, modelVertices3D, modelIndices3D);
                    renderer.CreateVertexBuffer<Vertex3D>(modelVertices3D); 
                    renderer.CreateIndexBuffer(modelIndices3D);
                }
            }
        }

        if (resource["Textures"]) {
            //texture id is allocated by engine, instead of user, in order
            for (const auto& texture : resource["Textures"]) {
                std::string name = texture["resource_texture_name"].as<std::string>();
                //int id = texture["resource_texture_id"].as<int>();
                int miplevel = texture["resource_texture_miplevels"].as<int>();
                bool enableCubemap = texture["resource_texture_cubmap"].as<bool>();
                int samplerid = texture["uniform_Sampler_id"].as<int>();

                VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                //VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                //for(int i = 0; i < textureAttributes->size(); i++){
                    //auto startTextureTime = std::chrono::high_resolution_clock::now();

                if(miplevel > 1) //mipmap
                    usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                else 
                    if(CComputeDescriptorManager::computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                    else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                
                if(!appInfo.Feature.b_feature_graphics_48pbt){ //24bpt
                    if(CComputeDescriptorManager::computeUniformTypes & COMPUTE_STORAGEIMAGE_SWAPCHAIN) textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, swapchain.swapChainImageFormat);
                    else textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R8G8B8A8_SRGB, 8, enableCubemap);  
                }else{ //48bpt
                    //textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R16G16B16A16_UNORM, 16, enableCubemap); 
                    textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R16G16B16A16_SFLOAT, 16, enableCubemap); 
                }
                
                if(appInfo.Feature.b_feature_graphics_rainbow_mipmap){
                    VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    if(miplevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps("checkerboard", usage_mipmap);
                }else if(miplevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps();
                
                    //auto endTextureTime = std::chrono::high_resolution_clock::now();
                    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
                    //std::cout<<"Load Texture '"<< (*textureNames)[i].first <<"' cost: "<<durationTime<<" milliseconds"<<std::endl;
                //}
            }
        }

        //shaders id is allocated by engine, not user, in order
        if (resource["Pipelines"]) {
            appInfo.VertexShader =  std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            appInfo.FragmentShader =  std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            appInfo.EnableSamplerCountOne =  std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.EnableDepthBias =  std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.Subpass =  std::make_unique<std::vector<int>>(std::vector<int>());

            for (const auto& pipeline : resource["Pipelines"]) {
                std::string name = pipeline["resource_graphics_pipeline_name"].as<std::string>();
                std::string vertexShaderName = pipeline["resource_graphics_pipeline_vertexshader_name"].as<std::string>();
                std::string fragmentShaderName = pipeline["resource_graphics_pipeline_fragmentshader_name"].as<std::string>();
                bool bEnableSamplerCountOne = pipeline["resource_graphics_pipeline_enable_sampler_count_one"] ? pipeline["resource_graphics_pipeline_enable_sampler_count_one"].as<bool>() : false;
                bool bEnableDepthBias = pipeline["resource_graphics_pipeline_enable_depth_bias"] ? pipeline["resource_graphics_pipeline_enable_depth_bias"].as<bool>() : false;
                int subpassId = pipeline["subpasses_subpass_id"] ? pipeline["subpasses_subpass_id"].as<int>() : 0;

                //std::cout<<"Pipeline Name: "<<name<<std::endl;
                appInfo.VertexShader->push_back(vertexShaderName);
                appInfo.FragmentShader->push_back(fragmentShaderName);
                appInfo.EnableSamplerCountOne->push_back(bEnableSamplerCountOne);
                appInfo.EnableDepthBias->push_back(bEnableDepthBias);
                appInfo.Subpass->push_back(subpassId);
            }

            // if (resource["VertexShaders"]) {
            //     auto vertexShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            //     for (const auto& vertexShader : resource["VertexShaders"]) {
            //         vertexShaderList->push_back(vertexShader["resource_vertexshader_name"].as<std::string>());
            //     }
            //     appInfo.VertexShader = std::move(vertexShaderList);
            // }

            // if (resource["FragmentShaders"]) {
            //     auto fragmentShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            //     for (const auto& fragmentShader : resource["FragmentShaders"]) {
            //         fragmentShaderList->push_back(fragmentShader["resource_fragmentshader_name"].as<std::string>());
            //     }
            //     appInfo.FragmentShader = std::move(fragmentShaderList);
            // }
        }

        if (resource["ComputeShaders"]) {
            auto computeShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& computeShader : resource["ComputeShaders"]) {
                computeShaderList->push_back(computeShader["resource_computeshader_name"].as<std::string>());
            }
            appInfo.ComputeShader = std::move(computeShaderList);
        }
    }
}

void CApplication::ReadAttachments(){
    bool bShadowmapAttachmentDepthLight = config["ShadowmapRenderpassAttachments"]["ShadowmapRenderpass_attachment_depth_light"] ? config["ShadowmapRenderpassAttachments"]["ShadowmapRenderpass_attachment_depth_light"].as<bool>() : false;
    bool bMainSceneAttachmentDepthLight = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_light"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_light"].as<bool>() : false;
    bool bMainSceneAttachmentDepthCamera = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_camera"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_camera"].as<bool>()  : false;
    bool bMainSceneAttachmentColorResovle = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_resovle"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_resovle"].as<bool>()  : false;
    bool bMainSceneAttachmentColorPresent = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_present"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_present"].as<bool>()  : true; //need al least one subpass with at least one color attachment

    renderProcess.iShadowmapAttachmentDepthLight = bShadowmapAttachmentDepthLight ? 0 : -1; //shadowmap renderpass attachment depth light, only one attachment, so id is 0

    int AttachmentCount = 0;
    renderProcess.iMainSceneAttachmentDepthLight = bMainSceneAttachmentDepthLight ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentDepthCamera = bMainSceneAttachmentDepthCamera ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentColorResovle = bMainSceneAttachmentColorResovle ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentColorPresent = bMainSceneAttachmentColorPresent ? AttachmentCount++ : -1;

    swapchain.iShadowmapAttachmentDepthLight = renderProcess.iShadowmapAttachmentDepthLight;
    swapchain.iMainSceneAttachmentDepthLight = renderProcess.iMainSceneAttachmentDepthLight;
    swapchain.iMainSceneAttachmentDepthCamera = renderProcess.iMainSceneAttachmentDepthCamera;
    swapchain.iMainSceneAttachmentColorResovle = renderProcess.iMainSceneAttachmentColorResovle;
    swapchain.iMainSceneAttachmentColorPresent = renderProcess.iMainSceneAttachmentColorPresent;

    std::cout<<"Shadowmap attachments: "<<swapchain.iShadowmapAttachmentDepthLight<<std::endl;
    std::cout<<"Mainscene attachments: "<<swapchain.iMainSceneAttachmentDepthLight<<","<<swapchain.iMainSceneAttachmentDepthCamera<<","<<swapchain.iMainSceneAttachmentColorResovle<<","<<swapchain.iMainSceneAttachmentColorPresent<<std::endl;

    //when creating attachment resource, need 1.create attachment description in renderProcess; 2.create attachment buffer in swapchain
    if(swapchain.iMainSceneAttachmentColorResovle >= 0) swapchain.GetMaxUsableSampleCount(); //calcuate max sampler count first

    //If enable MSAA, must also enable Depth Test
    if(swapchain.iMainSceneAttachmentDepthCamera >= 0){
        swapchain.create_attachment_resource_depth_camera();
        renderProcess.create_attachment_description_camera_depth_mainsceneRenderPass(swapchain.depthFormat, swapchain.msaaSamples);
    }

    if(swapchain.iMainSceneAttachmentColorResovle >= 0){
        swapchain.create_attachment_resource_color_resolve();
        renderProcess.create_attachment_description_color_resolve_mainsceneRenderPass(swapchain.swapChainImageFormat, swapchain.msaaSamples, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

   
    //bool bEnableSamplerCountOne = appInfo.RenderMode == renderer.GRAPHICS_SHADOWMAP ? true : false; //if use shadowmap(2pass), then depthLight attachment must use samper count 1
    if(swapchain.iShadowmapAttachmentDepthLight >= 0){ //if shadowmap renderpass attachment depth light is enabled
        swapchain.create_attachment_resource_depth_light(true); //hardware bias todo
        //renderProcess.create_attachment_description_light_depth_mainsceneRenderPass(swapchain.depthFormat, VK_SAMPLE_COUNT_1_BIT); //hardware bias todo
        renderProcess.create_attachment_description_light_depth_shadowmapRenderPass(swapchain.depthFormat, VK_SAMPLE_COUNT_1_BIT); //hardware bias todo//should remove the second parameter
    }else if(swapchain.iMainSceneAttachmentDepthLight >= 0){
        swapchain.create_attachment_resource_depth_light(false);
        renderProcess.create_attachment_description_light_depth_mainsceneRenderPass(swapchain.depthFormat, swapchain.msaaSamples);
    }

    if(swapchain.iMainSceneAttachmentColorPresent >= 0) //dont need create swapchain attachment resource here
        renderProcess.create_attachment_description_color_present_mainsceneRenderPass(swapchain.swapChainImageFormat);
}

void CApplication::ReadSubpasses(){
    renderProcess.bEnableShadowmapRenderpassSubpassShadowmap = config["ShadowmapRenderpassSubpasses"]["shadowmapRenderpass_subpasses_shadowmap"] ? config["ShadowmapRenderpassSubpasses"]["shadowmapRenderpass_subpasses_shadowmap"].as<bool>() : false;
    renderProcess.bEnableMainSceneRenderpassSubpassShadowmap = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_shadowmap"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_shadowmap"].as<bool>() : false;
    renderProcess.bEnableMainSceneRenderpassSubpassDraw = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_draw"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_draw"].as<bool>() : true; //need at least one subpass, even for compute sample
    renderProcess.bEnableMainSceneRenderpassSubpassObserve = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_observe"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_observe"].as<bool>() : false;

    //for shadowmap renderpass (this renderpass is optional)
    if(renderProcess.bEnableShadowmapRenderpassSubpassShadowmap){
        // std::cout<<"Application: Create Shadowmap Render Pass."<<std::endl;
        renderProcess.createSubpass_shadowmapRenderpass();
        renderProcess.createDependency_shadowmapRenderpass();
        renderProcess.createRenderPass_shadowmapRenderpass();

        // std::cout<<"Application: Create Shadowmap Framebuffer."<<std::endl;
        swapchain.CreateFramebuffer_shadowmap(renderProcess.renderPass_shadowmap);
    }

    //for mainscene renderpass (this renderpass is mandatory)
    //create renderpass
    std::cout<<"Application: Create MainScene Render Pass."<<std::endl;
    renderProcess.createSubpass_mainsceneRenderpass(appInfo.Feature.feature_graphics_observe_attachment_id);
    renderProcess.createDependency_mainsceneRenderpass();
    renderProcess.createRenderPass_mainsceneRenderpass();

    //create framebuffer
    std::cout<<"Application: Create MainScene Framebuffer."<<std::endl;
    swapchain.CreateFramebuffer_mainscene(renderProcess.renderPass_mainscene);
}

void CApplication::CreateUniformDescriptors(bool b_uniform_graphics, bool b_uniform_compute){
    //UNIFORM STEP 1/3 (Pool)
    CGraphicsDescriptorManager::createDescriptorPool(objects.size()); 
    CComputeDescriptorManager::createDescriptorPool(); 

    //UNIFORM STEP 2/3 (Layer)
    if(b_uniform_graphics){
        if(appInfo.Uniform.b_uniform_graphics_custom) 
             CGraphicsDescriptorManager::createDescriptorSetLayout_General(&appInfo.Uniform.GraphicsCustom.Binding); 
        else CGraphicsDescriptorManager::createDescriptorSetLayout_General(); 
        if(CGraphicsDescriptorManager::textureImageSamplers.size()>0) CGraphicsDescriptorManager::createDescriptorSetLayout_TextureImageSampler(); 
    }
    if(b_uniform_compute){
        if(appInfo.Uniform.b_uniform_compute_custom) CComputeDescriptorManager::createDescriptorSetLayout(&appInfo.Uniform.ComputeCustom.Binding);
        else CComputeDescriptorManager::createDescriptorSetLayout();
    }

    //UNIFORM STEP 3/3 (Set)
    if(b_uniform_graphics){
        //if(appInfo.Feature.feature_graphics_observe_attachment_id == 0) //assume 0 is light Depth Image Buffer
            graphicsDescriptorManager.createDescriptorSets_General(swapchain.depthImageBuffer.view, swapchain.lightDepthImageBuffer.view, swapchain.lightDepthImageBuffer.view); //TODO
           // graphicsDescriptorManager.createDescriptorSets_General(swapchain.depthImageBuffer.view, swapchain.lightDepthImageBuffer.view); //TODO
        //else// if(appInfo.Feature.feature_graphics_observe_attachment_id == 1)
            //graphicsDescriptorManager.createDescriptorSets_General(swapchain.depthImageBuffer.view);//TODO: what if no depthImageBuffer is not enable 
    }
    if(b_uniform_compute){
        if(appInfo.Uniform.b_uniform_compute_swapchain_storage) {
            if(appInfo.Uniform.b_uniform_compute_texture_storage)
                computeDescriptorManager.createDescriptorSets(&(textureManager.textureImages), &(swapchain.views));//this must be called after texture resource is loaded
            else computeDescriptorManager.createDescriptorSets(NULL, &(swapchain.views));
        }else computeDescriptorManager.createDescriptorSets();
    }
}

void CApplication::CreatePipelines(){
    bool bVerbose = true;

    /****************************
    * Command Buffer
    ****************************/
    if(appInfo.VertexShader != NULL) renderer.CreateGraphicsCommandBuffer();
    if(appInfo.ComputeShader != NULL) renderer.CreateComputeCommandBuffer();
    if(bVerbose) std::cout<<"CreatePipeline: Done Command Buffer"<<std::endl;

    /****************************
    * Frame Buffer (legacy)
    ****************************/
    //if(appInfo.VertexShader != NULL){
        // VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // renderProcess.enableColorAttachmentDescriptionColorPresent(swapchain.swapChainImageFormat);//assume when vertex is non-null, need a color attachment for presentation(must be single sampled)
        // if(swapchain.bEnableDepthTest) {
        //     renderProcess.enableAttachmentDescriptionDepth(swapchain.depthFormat, swapchain.msaaSamples);
        //     if(swapchain.bEnableMSAA) //if enable MSAA, must also enable depthTest
        //         renderProcess.enableAttachmentDescriptionColorMultiSample(swapchain.swapChainImageFormat, swapchain.msaaSamples, imageLayout); 
        // }
        // //renderProcess.createSubpass();
        // if(swapchain.bEnableDepthTest){
        //     VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //     VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //     renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
        // }else renderProcess.createDependency();
        // renderProcess.createRenderPass();

        //swapchain.CreateFramebuffers(renderProcess.renderPass);
    //}
    //if(bVerbose) std::cout<<"CreatePipeline: Done Frame Buffer"<<std::endl;
    
    /****************************
    * Create Shaders
    ****************************/
    if(appInfo.VertexShader != NULL){
        for(int i = 0; i < appInfo.VertexShader->size(); i++){
            shaderManager.CreateShader((*appInfo.VertexShader)[i], shaderManager.VERT);
            shaderManager.CreateShader((*appInfo.FragmentShader)[i], shaderManager.FRAG);
        }
    }
    if(appInfo.ComputeShader != NULL)
        for(int i = 0; i < appInfo.ComputeShader->size(); i++)
            shaderManager.CreateShader((*appInfo.ComputeShader)[i], shaderManager.COMP);
    if(bVerbose) std::cout<<"CreatePipeline: Done Create Shaders"<<std::endl;

    /****************************
    * Create Pipelines
    ****************************/
    if(appInfo.VertexShader != NULL){
        std::vector<VkDescriptorSetLayout> dsLayouts; //2 sets for graphics

        if((CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM) || 
            (CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING) || 
            (CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP) ||
            (CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP)){
            if(bVerbose) std::cout<<"CreatePipeline: Add layout set0: graphics general layout"<<std::endl;
            dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout_general); //set = 0
        }

        if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE) {
            if(bVerbose) std::cout<<"CreatePipeline: Add layout set1: sampler(texture) layout"<<std::endl;
            dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout_textureImageSampler); //set = 1
        }

  
        //Different cube can share the same texture descriptor.
        //suppose we have 100 objects, 100 different textures. cube x 50, sphere x 50. How many texture layouts? How many texture descriptor?
        //obviously, every objects need a different texture, so bind with objectId
        //but for layout, can use one. That means texture layout should be object property, while the descriptor set(associate with image) should be cube[i]/sphere[i] bound

        //each object can have muti texture image, multi descriptor set(when creating descritpor set, need a sampler)
        //all objects share the same descriptor pool and descriptor layout, they are universal
        //sampler should also be universal
        
        //std::cout<<"Begin create graphics pipeline"<<std::endl;
        for(int i = 0; i < appInfo.VertexShader->size(); i++){
            //std::cout<<"test create pipeline"<<std::endl;
            //! All graphics pipelines use the same dsLayouts
            if(shaderManager.bEnablePushConstant){
                if(bVerbose) std::cout<<"CreatePipeline: Try Create Push Constant Layout"<<std::endl;
                renderProcess.createGraphicsPipelineLayout(dsLayouts,  shaderManager.pushConstantRange, true, i);
                if(bVerbose) std::cout<<"CreatePipeline: Done Create Push Constant Layout"<<std::endl;
            }
            else renderProcess.createGraphicsPipelineLayout(dsLayouts, i);

            if(bVerbose) std::cout<<"CreatePipeline: Try Create graphics pipeline: "<<i<<", VertexStructureType="<<appInfo.VertexBufferType<<std::endl;
            switch(appInfo.VertexBufferType){
                case VertexStructureTypes::NoType:
                    renderProcess.createGraphicsPipeline(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], i,
                        (*appInfo.Subpass)[i], (*appInfo.EnableSamplerCountOne)[i], (*appInfo.EnableDepthBias)[i], renderProcess.renderPass_mainscene);  
                break;
                case VertexStructureTypes::ThreeDimension:
                    //TODO: for 2-renderpass case, each pipeline for different renderpass
                    // if(i == 0)
                    // renderProcess.createGraphicsPipeline<Vertex3D>(
                    //     VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                    //     shaderManager.vertShaderModules[i], 
                    //     shaderManager.fragShaderModules[i], true, i,
                    //     (*appInfo.Subpass)[i], (*appInfo.EnableSamplerCountOne)[i], (*appInfo.EnableDepthBias)[i], renderProcess.renderPass_shadowmap);  
                    // else
                    renderProcess.createGraphicsPipeline<Vertex3D>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], true, i,
                        (*appInfo.Subpass)[i], (*appInfo.EnableSamplerCountOne)[i], (*appInfo.EnableDepthBias)[i], renderProcess.renderPass_mainscene);  
                break;
                case VertexStructureTypes::TwoDimension:
                    renderProcess.createGraphicsPipeline<Vertex2D>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], true, i,
                        (*appInfo.Subpass)[i], (*appInfo.EnableSamplerCountOne)[i], (*appInfo.EnableDepthBias)[i], renderProcess.renderPass_mainscene);
                break;
                case VertexStructureTypes::ParticleType:
                    renderProcess.createGraphicsPipeline<Particle>(
                        VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], true, i,
                        (*appInfo.Subpass)[i], (*appInfo.EnableSamplerCountOne)[i], (*appInfo.EnableDepthBias)[i], renderProcess.renderPass_mainscene);
                break;
                default:
                break;
            }
        }
        //std::cout<<"Done create graphics pipeline"<<std::endl;
    }
    if(appInfo.ComputeShader != NULL){ //for now assume only one compute pipeline
        //! only support one compute pipeline
        renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
        renderProcess.createComputePipeline(shaderManager.compShaderModules[0]);
    }
    if(bVerbose) std::cout<<"CreatePipeline: Done Create Pipelines"<<std::endl;
}

void CApplication::ReadRegisterObjects(){
    if (config["Objects"]) {
        //std::cerr << "No 'Objects' key found in the YAML file!" << std::endl;
        for (const auto& obj : config["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            if(objects[object_id].bRegistered) {
                std::cout<<"WARNING: Trying to register a registered Object id("<<object_id<<")!"<<std::endl;
                continue;
            }

            //std::cout<<"before register Object id("<<object_id<<")!"<<std::endl;
            int resource_model_id = obj["resource_model_id"] ? obj["resource_model_id"].as<int>() : 0;
            auto resource_texture_id_list = obj["resource_texture_id_list"] ? obj["resource_texture_id_list"].as<std::vector<int>>() : std::vector<int>(1, 0);
            int resource_graphics_pipeline_id_mainscene = obj["resource_graphics_pipeline_id_mainscene"] ? obj["resource_graphics_pipeline_id_mainscene"].as<int>() : 0;
            int resource_graphics_pipeline_id_mainscene2 = obj["resource_graphics_pipeline_id_mainscene2"] ? obj["resource_graphics_pipeline_id_mainscene2"].as<int>() : 0;
            int resource_graphics_pipeline_id_shadowmap = obj["resource_graphics_pipeline_id_shadowmap"] ? obj["resource_graphics_pipeline_id_shadowmap"].as<int>() : 0;
            //must load resources before object register
            objects[object_id].Register((CApplication*)this, object_id, resource_texture_id_list, resource_model_id, resource_graphics_pipeline_id_mainscene, resource_graphics_pipeline_id_mainscene2,resource_graphics_pipeline_id_shadowmap);
            //std::cout<<"after register Object id("<<object_id<<")!"<<std::endl;

            std::string name = obj["object_name"] ? obj["object_name"].as<std::string>() : "Default";
            objects[object_id].Name = name;

            //set scale after model is registered, otherwise the length will not be computed correctly
            float object_scale = obj["object_scale"] ? obj["object_scale"].as<float>() : 1.0f;
            objects[object_id].SetScale(object_scale);

            auto position = obj["object_position"] ? obj["object_position"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objects[object_id].SetPosition(position[0], position[1], position[2]);

            auto rotation = obj["object_rotation"] ? obj["object_rotation"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objects[object_id].SetRotation(rotation[0], rotation[1], rotation[2]);

            auto velocity = obj["object_velocity"] ? obj["object_velocity"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objects[object_id].SetVelocity(velocity[0], velocity[1], velocity[2]);

            auto angular_velocity = obj["object_angular_velocity"] ? obj["object_angular_velocity"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objects[object_id].SetAngularVelocity(angular_velocity[0], angular_velocity[1], angular_velocity[2]);

            bool isSkybox = obj["object_skybox"] ? obj["object_skybox"].as<bool>() : false;
            objects[object_id].bSkybox = isSkybox;
            //if(graphics_pipeline_id == appInfo.Feature.GraphicsPipelineSkyboxID)  objectList[i].bSkybox = true;

            std::cout<<"ObjectId:("<<object_id<<") Name:("<<objects[object_id].Name<<") Length:("<<objects[object_id].Length.x<<","<<objects[object_id].Length.y<<","<<objects[object_id].Length.z<<")"
                <<" Position:("<<objects[object_id].Position.x<<","<<objects[object_id].Position.y<<","<<objects[object_id].Position.z<<")"<<std::endl;
        }
        for(int i = 0; i < objects.size(); i++)
            if(!objects[i].bRegistered) std::cout<<"WARNING: Object id("<<i<<") is not registered!"<<std::endl;
    }
}

void CApplication::ReadLightings(){
    if (config["Lights"]) {
        for (const auto& light : config["Lights"]) {
            int id = light["light_id"] ? light["light_id"].as<int>() : 0;
            if(lights[id].bRegistered) {
                std::cout<<"WARNING: Trying to register a registered Light id("<<id<<")!"<<std::endl;
                continue;
            }
            
            std::string name = light["light_name"] ? light["light_name"].as<std::string>() : "Default";

            auto position = light["light_position"] ? light["light_position"].as<std::vector<float>>(): std::vector<float>(3,0);
            glm::vec3 glm_position(position[0], position[1], position[2]);
            auto intensity = light["light_intensity"] ? light["light_intensity"].as<std::vector<float>>(): std::vector<float>(4,0);

            lights[id].Register(name, id, glm_position, intensity);

            std::cout<<"LightId:("<<id<<") Name:("<<lights[id].GetLightName()<<") Intensity:("<<lights[id].GetIntensity(0)<<","<<lights[id].GetIntensity(1)<<","<<lights[id].GetIntensity(2)<<","<<lights[id].GetIntensity(3)<<")"
                <<" Position:("<<lights[id].GetLightPosition().x<<","<<lights[id].GetLightPosition().y<<","<<lights[id].GetLightPosition().z<<")"<<std::endl;
 
        }
        for(int i = 0; i < lights.size(); i++)
            if(!lights[i].bRegistered) std::cout<<"WARNING: Light id("<<i<<") is not registered!"<<std::endl;
    }
}

void CApplication::ReadMainCamera(){
    mainCamera.cameraType = (Camera::CameraType)(config["MainCamera"]["camera_mode"] ? config["MainCamera"]["camera_mode"].as<int>() : 0);
    mainCamera.SetPosition(
        config["MainCamera"]["camera_position"][0].as<float>(), 
        config["MainCamera"]["camera_position"][1].as<float>(), 
        config["MainCamera"]["camera_position"][2].as<float>());
    mainCamera.SetRotation(
        config["MainCamera"]["camera_rotation"][0].as<float>(), 
        config["MainCamera"]["camera_rotation"][1].as<float>(), 
        config["MainCamera"]["camera_rotation"][2].as<float>());
        
    focusObjectId = config["MainCamera"]["object_id_target"] ? config["MainCamera"]["object_id_target"].as<int>() : 0;
    // mainCamera.SetTargetPosition(
    //     config["MainCamera"]["camera_target_position"][0].as<float>(), 
    //     config["MainCamera"]["camera_target_position"][1].as<float>(), 
    //     config["MainCamera"]["camera_target_position"][2].as<float>());
    mainCamera.setPerspective(
        config["MainCamera"]["camera_fov"].as<float>(),  
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
        config["MainCamera"]["camera_z"][0].as<float>(), 
        config["MainCamera"]["camera_z"][1].as<float>());

#ifdef SDL
    sdlManager.keyboard_sensitive = config["MainCamera"]["camera_keyboard_sensitive"] ? config["MainCamera"]["camera_keyboard_sensitive"].as<float>() : 3;
    sdlManager.mouse_sensitive = config["MainCamera"]["camera_mouse_sensitive"] ? config["MainCamera"]["camera_mouse_sensitive"].as<float>() : 60;
#else    
    glfwManager.keyboard_sensitive = config["MainCamera"]["camera_keyboard_sensitive"] ? config["MainCamera"]["camera_keyboard_sensitive"].as<float>() : 3;
    glfwManager.mouse_sensitive = config["MainCamera"]["camera_mouse_sensitive"] ? config["MainCamera"]["camera_mouse_sensitive"].as<float>() : 60;
#endif
 
    lightCamera.cameraType = mainCamera.cameraType;
    lightCamera.SetPosition(mainCamera.Position);
    lightCamera.SetRotation(mainCamera.Rotation);
    lightCamera.setPerspective(mainCamera.fov,  (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, mainCamera.znear, mainCamera.zfar);
    //lightCamera.setPerspective(90, 1.0f, 0.1f, 6);
    //lightCamera.setPerspective(90, 1.0f, 0.1f, 5);
    //lightCamera.setPerspective(60, 1.0f, 0.5f, 10.0f); //TODO

    // lightCamera.setOrthographic(
    //     -10, 10, 
    //     -10, 10, 
    //     0.1, 100);
    //lightCamera.matrices.perspective[1][1] *= -1;
}

void CApplication::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    //CSupervisor::Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    dsSets.push_back(computeDescriptorManager.descriptorSets);

    renderer.BindComputeDescriptorSets(renderProcess.computePipelineLayout, dsSets, -1); //-1 to offset means no dynamic offset

    //std::cout<<"Record Compute command buffer. "<<std::endl;
    renderer.Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
}