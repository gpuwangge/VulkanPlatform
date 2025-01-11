#include "../include/application.h"

//static class members must be defined outside. 
//otherwise invoke 'undefined reference' error when linking
Camera CApplication::mainCamera;
bool CApplication::NeedToExit = false; 
bool CApplication::NeedToPause = false;
int CApplication::LightCount = 0;
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

    //Handle uniform yaml data
    appInfo.Uniform.GraphicsVector = config["Uniform"]["Graphics"].as<std::vector<std::vector<bool>>>();
    appInfo.Uniform.ComputeVector = config["Uniform"]["Compute"].as<std::vector<std::vector<bool>>>();
    appInfo.Uniform.SamplerMiplevels = config["Uniform"]["SamplerMiplevels"].as<std::vector<int>>();

    //Handle feature yaml data
    appInfo.Feature.EnableGraphicsDepthTest = config["Feature"]["GraphicsDepthTest"].as<bool>();
    appInfo.Feature.EnableGraphicsMSAA = config["Feature"]["GraphicsMSAA"].as<bool>();
    appInfo.Feature.EnableGraphics48BPT = config["Feature"]["Graphics48BPT"].as<bool>();
    appInfo.Feature.EnableGraphicsPushConstant = config["Feature"]["GraphicsPushConstant"].as<bool>();
    appInfo.Feature.EnableGraphicsBlend = config["Feature"]["GraphicsBlend"].as<bool>();
    appInfo.Feature.EnableGraphicsRainbowMipmap = config["Feature"]["GraphicsRainbowMipmap"].as<bool>();
    appInfo.Feature.GraphicsPipelineSkyboxID = config["Feature"]["GraphicsPipelineSkyboxID"].as<int>();
   
    //Handle camera yaml data
    if(config["MainCamera"]["FreeMode"].as<bool>() == true) mainCamera.cameraType = Camera::CameraType::freemove;
    mainCamera.SetTargetPosition(config["MainCamera"]["TargetLocation"][0].as<float>(), config["MainCamera"]["TargetLocation"][1].as<float>(), config["MainCamera"]["TargetLocation"][2].as<float>());
    mainCamera.SetPosition(config["MainCamera"]["Position"][0].as<float>(), config["MainCamera"]["Position"][1].as<float>(), config["MainCamera"]["Position"][2].as<float>());
    mainCamera.SetRotation(config["MainCamera"]["Rotation"][0].as<float>(), config["MainCamera"]["Rotation"][1].as<float>(), config["MainCamera"]["Rotation"][2].as<float>());
    mainCamera.setPerspective(config["MainCamera"]["FOV"].as<float>(),  (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
        config["MainCamera"]["Z"][0].as<float>(), config["MainCamera"]["Z"][1].as<float>());

    //Handle Lighting data
    if(config["Lighting"]["Position"].size() > 0) {
        std::unique_ptr<std::vector<std::vector<float>>> lightPosition = std::make_unique<std::vector<std::vector<float>>>(config["Lighting"]["Position"].as<std::vector<std::vector<float>>>());
        for(int i = 0; i < lightPosition->size(); i++)
            graphicsDescriptorManager.m_lightingUBO.lights[i].lightPos = glm::vec4(glm::vec3((*lightPosition)[i][0], (*lightPosition)[i][1], (*lightPosition)[i][2]), 0);

        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_LIGHTING_GRAPHICS_BIT;
        CGraphicsDescriptorManager::addLightingUniformBuffer();
        LightCount = lightPosition->size();
    }
    if(config["Lighting"]["Intensity"].size() > 0) {
        std::unique_ptr<std::vector<std::vector<float>>> lightIntensity = std::make_unique<std::vector<std::vector<float>>>(config["Lighting"]["Intensity"].as<std::vector<std::vector<float>>>());
        for(int i = 0; i < lightIntensity->size(); i++){
            graphicsDescriptorManager.m_lightingUBO.lights[i].ambientIntensity = (*lightIntensity)[i][0];
            graphicsDescriptorManager.m_lightingUBO.lights[i].diffuseIntensity = (*lightIntensity)[i][1];
            graphicsDescriptorManager.m_lightingUBO.lights[i].specularIntensity = (*lightIntensity)[i][2];
            graphicsDescriptorManager.m_lightingUBO.lights[i].dimmerSwitch = (*lightIntensity)[i][3];
        }
    }
    
    //Real Initialization Starts Here
    //each object should have a pipeline reference, so can use the pipeline size as object size. 
    //must set this before Set App Property(because of descriptor size rely on object size)
    //particle sample has one object

    /****************************
    * Initialize ObjectList
    ****************************/
    if (config["Objects"]) {
        int objectSize = 0;
        for (const auto& obj : config["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            objectSize = (object_id > objectSize) ? object_id : objectSize;
        }
        objectList.resize(((objectSize+1) < config["Objects"].size())?(objectSize+1):config["Objects"].size()); 
        std::cout<<"Object Number: "<<objectList.size()<<std::endl;
    }

     /****************************
    * Set Application Property
    ****************************/   
   //CSupervisor::VertexStructureType = VertexStructureTypes::ThreeDimension;//?
    //auto startAppTime = std::chrono::high_resolution_clock::now();
    SetApplicationProperty(appInfo);
    //auto endAppTime = std::chrono::high_resolution_clock::now();
    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endAppTime - startAppTime).count() * 1000;
    //std::cout<<"Total Set Application Property cost: "<<durationTime<<" milliseconds"<<std::endl;

    /****************************
    * Read Resources
    ****************************/
    for (const auto& resource : config["Resources"]) {
        if (resource["Models"]) {
            for (const auto& model : resource["Models"]) {
                std::string name = model["resource_model_name"] ? model["resource_model_name"].as<std::string>() : "Default";
                //std::cout<<"model name: "<<name<<std::endl;
                //id is not really useful here, because the model id must be in order
                int id = model["resource_model_id"] ? model["resource_model_id"].as<int>() : 0;

                CSupervisor::VertexStructureType = VertexStructureTypes::ThreeDimension;
                if(name == "CUSTOM3D0"){
                    renderer.CreateVertexBuffer<Vertex3D>(modelManager.customModels3D[0].vertices); 
                    renderer.CreateIndexBuffer(modelManager.customModels3D[0].indices);
                    
                    modelManager.modelLengths.push_back(modelManager.customModels3D[0].length);
                    modelManager.modelLengthsMin.push_back(modelManager.customModels3D[0].lengthMin);
                    modelManager.modelLengthsMax.push_back(modelManager.customModels3D[0].lengthMax);
                }else if(name == "CUSTOM2D0"){
                    CSupervisor::VertexStructureType = VertexStructureTypes::TwoDimension;
                    renderer.CreateVertexBuffer<Vertex2D>(modelManager.customModels2D[0].vertices); 

                    modelManager.modelLengths.push_back(modelManager.customModels2D[0].length);
                    modelManager.modelLengthsMin.push_back(modelManager.customModels2D[0].lengthMin);
                    modelManager.modelLengthsMax.push_back(modelManager.customModels2D[0].lengthMax);
                }else{
                    CSupervisor::VertexStructureType = VertexStructureTypes::ThreeDimension;
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
                    if(CSupervisor::Query_Uniform_Compute_StorageImage()) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                    else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                if(!CSupervisor::b48bpt) //24bpt
                    if(CSupervisor::Query_Uniform_Compute_StorageImage_Swapchain()) textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, swapchain.swapChainImageFormat);
                    else textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R8G8B8A8_SRGB, 8, enableCubemap);  
                else //48bpt
                    textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R16G16B16A16_UNORM, 16, enableCubemap); 
                
                if(CSupervisor::bRainbowMipmap){
                    VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    if(miplevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps("checkerboard", usage_mipmap);
                }else if(miplevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps();
                
                    //auto endTextureTime = std::chrono::high_resolution_clock::now();
                    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
                    //std::cout<<"Load Texture '"<< (*textureNames)[i].first <<"' cost: "<<durationTime<<" milliseconds"<<std::endl;
                //}
            }
        }

        //std::cout<<"test1"<<std::endl;
        //shaders id is allocated by engine, not user, in order

        if (resource["VertexShaders"]) {
            auto vertexShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& vertexShader : resource["VertexShaders"]) {
                vertexShaderList->push_back(vertexShader["resource_vertexshader_name"].as<std::string>());
            }
            appInfo.Object.Pipeline.VertexShader = std::move(vertexShaderList);
            std::cout<<"vertex shader:"<<appInfo.Object.Pipeline.VertexShader->size()<<std::endl;
        }

        if (resource["FragmentShaders"]) {
            auto fragmentShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& fragmentShader : resource["FragmentShaders"]) {
                fragmentShaderList->push_back(fragmentShader["resource_fragmentshader_name"].as<std::string>());
            }
            appInfo.Object.Pipeline.FragmentShader = std::move(fragmentShaderList);
        }

        if (resource["ComputeShaders"]) {
            auto computeShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& computeShader : resource["ComputeShaders"]) {
                computeShaderList->push_back(computeShader["resource_computeshader_name"].as<std::string>());
            }
            appInfo.Object.Pipeline.ComputeShader = std::move(computeShaderList);
        }
    }

    //std::cout<<"test2"<<std::endl;

    CSupervisor::Activate_Pipeline();

    /****************************
    * Read and Register Objects
    ****************************/
    if (config["Objects"]) {
        //std::cerr << "No 'Objects' key found in the YAML file!" << std::endl;
        for (const auto& obj : config["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            if(objectList[object_id].bRegistered) {
                std::cout<<"WARNING: Trying to register a registered Object id("<<object_id<<")!"<<std::endl;
                continue;
            }

            //std::cout<<"before register Object id("<<object_id<<")!"<<std::endl;
            int resource_model_id = obj["resource_model_id"] ? obj["resource_model_id"].as<int>() : 0;
            auto resource_texture_id_list = obj["resource_texture_id_list"] ? obj["resource_texture_id_list"].as<std::vector<int>>() : std::vector<int>(1, 0);
            int resource_graphics_pipeline_id = obj["resource_graphics_pipeline_id"] ? obj["resource_graphics_pipeline_id"].as<int>() : 0;
            //must load resources before object register
            objectList[object_id].Register((CApplication*)this, object_id, resource_texture_id_list, resource_model_id, resource_graphics_pipeline_id);
            //std::cout<<"after register Object id("<<object_id<<")!"<<std::endl;

            std::string name = obj["object_name"] ? obj["object_name"].as<std::string>() : "Default";
            objectList[object_id].Name = name;

            //set scale after model is registered, otherwise the length will not be computed correctly
            float object_scale = obj["object_scale"] ? obj["object_scale"].as<float>() : 1.0f;
            objectList[object_id].SetScale(object_scale);

            auto position = obj["object_position"] ? obj["object_position"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objectList[object_id].SetPosition(position[0], position[1], position[2]);

            auto rotation = obj["object_rotation"] ? obj["object_rotation"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objectList[object_id].SetRotation(rotation[0], rotation[1], rotation[2]);

            auto velocity = obj["object_velocity"] ? obj["object_velocity"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objectList[object_id].SetVelocity(velocity[0], velocity[1], velocity[2]);

            auto angular_velocity = obj["object_angular_velocity"] ? obj["object_angular_velocity"].as<std::vector<float>>(): std::vector<float>(3, 0);
            objectList[object_id].SetAngularVelocity(angular_velocity[0], angular_velocity[1], angular_velocity[2]);

            bool isSkybox = obj["object_skybox"] ? obj["object_skybox"].as<bool>() : false;
            objectList[object_id].bSkybox = isSkybox;
            //if(graphics_pipeline_id == appInfo.Feature.GraphicsPipelineSkyboxID)  objectList[i].bSkybox = true;

            std::cout<<"id:("<<object_id<<") "<<objectList[object_id].Name<<" Length:("<<objectList[object_id].Length.x<<","<<objectList[object_id].Length.y<<","<<objectList[object_id].Length.z<<")"
                <<" Position:("<<objectList[object_id].Position.x<<","<<objectList[object_id].Position.y<<","<<objectList[object_id].Position.z<<")"<<std::endl;
        }
        for(int i = 0; i < objectList.size(); i++)
            if(!objectList[i].bRegistered) std::cout<<"WARNING: Object id("<<i<<") is not registered!"<<std::endl;
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
    if(appInfo.Uniform.SamplerMiplevels.size() > 0) CSupervisor::Activate_Uniform_Graphics_Sampler(); //samplerCount
    
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

    // auto startLoadModelTime = std::chrono::high_resolution_clock::now();
    // if(appInfo.Object.Model.Names != NULL) CSupervisor::Activate_Buffer_Graphics_Vertex(std::move(appInfo.Object.Model.Names), modelManager); 
    // auto endLoadModelTime = std::chrono::high_resolution_clock::now();
    // auto durationLoadModelTime = std::chrono::duration<float, std::chrono::seconds::period>(endLoadModelTime - startLoadModelTime).count()*1000;
    // std::cout<<"Load Model cost: "<<durationLoadModelTime<<" milliseconds"<<std::endl;

    // auto startTextureTime = std::chrono::high_resolution_clock::now();
    // if(appInfo.Object.Texture.Attributes != NULL)
    //     CSupervisor::Activate_Texture(std::move(appInfo.Object.Texture.Attributes));
    // auto endTextureTime = std::chrono::high_resolution_clock::now();
    // auto durationTextureTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
    // std::cout<<"Load Textures cost: "<<durationTextureTime<<" milliseconds"<<std::endl;

    //auto startPipelineTime = std::chrono::high_resolution_clock::now();
   // CSupervisor::Activate_Pipeline();
    //auto endPipelineTime = std::chrono::high_resolution_clock::now();
    //durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endPipelineTime - startPipelineTime).count()*1000;
    //std::cout<<"Activate Pipeline cost: "<<durationTime<<" milliseconds"<<std::endl;
    
}
