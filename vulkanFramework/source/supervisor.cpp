#include "../include/supervisor.h"
#include "../include/application.h"

//Declare static variables here:
CApplication *CSupervisor::m_app;
//std::string CSupervisor::VertexShader;
//std::string CSupervisor::FragmentShader;
//std::string CSupervisor::ComputeShader;
std::vector<Vertex2D> CSupervisor::vertices2D;
//std::vector<Vertex3D> CSupervisor::vertices3D;
//std::vector<uint32_t> CSupervisor::indices3D;
VkDeviceSize CSupervisor::GraphicsCustomUniformBufferSize;
VkDescriptorSetLayoutBinding CSupervisor::GraphicsCustomBinding;
VkDeviceSize CSupervisor::ComputeStorageBufferSize;
VkBufferUsageFlags CSupervisor::ComputeStorageBufferUsage;
VkDeviceSize CSupervisor::ComputeCustomUniformBufferSize;
VkDescriptorSetLayoutBinding CSupervisor::ComputeCustomBinding;
//int CSupervisor::m_samplerCount;
//bool CMastermind::bDepthTest;
//bool CMastermind::bMSAA;
bool CSupervisor::b48bpt;
bool CSupervisor::bPushConstant;
bool CSupervisor::bBlend;
VertexStructureTypes CSupervisor::VertexStructureType;
bool CSupervisor::bRainbowMipmap;

void CSupervisor::Register(CApplication *app){ m_app = app; m_app->renderer.CreateCommandPool(m_app->surface);} 

bool CSupervisor::Query_Pipeline_Graphics(){ return m_app->appInfo.Object.Pipeline.VertexShader->size()>0; } //VertexShader!="";
bool CSupervisor::Query_Pipeline_Compute(){ return m_app->appInfo.Object.Pipeline.ComputeShader->size()>0; }//ComputeShader!="";

void CSupervisor::Activate_Uniform_Graphics_VP(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_VP_BIT; }
bool CSupervisor::Query_Uniform_Graphics_VP(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_VP_BIT;}
void CSupervisor::Activate_Uniform_Graphics_MVP(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_MVP_BIT;}
bool CSupervisor::Query_Uniform_Graphics_MVP(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_MVP_BIT;}
void CSupervisor::Activate_Uniform_Graphics_Custom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding){ 
    CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT;
    GraphicsCustomUniformBufferSize = graphicsCustomUniformBufferSize;
    GraphicsCustomBinding = graphicsCustomBinding;
}
bool CSupervisor::Query_Uniform_Graphics_Custom(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT;}
void CSupervisor::Activate_Uniform_Graphics_Sampler(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_SAMPLER_BIT;}
bool CSupervisor::Query_Uniform_Graphics_Sampler(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT;}

void CSupervisor::Activate_Uniform_Compute_Custom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding){ 
    CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;
    ComputeCustomUniformBufferSize = graphicsCustomUniformBufferSize;
    ComputeCustomBinding = graphicsCustomBinding; 
}
bool CSupervisor::Query_Uniform_Compute_Custom(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;}
void CSupervisor::Activate_Uniform_Compute_StorageBuffer(VkDeviceSize computeStorageBufferSize, VkBufferUsageFlags storageBufferUsage){ 
    CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_STORAGE_BIT;
    ComputeStorageBufferSize = computeStorageBufferSize;
    ComputeStorageBufferUsage = storageBufferUsage;
}
bool CSupervisor::Query_Uniform_Compute_StorageBuffer(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_BIT;}
void CSupervisor::Activate_Uniform_Compute_StorageImage(){  //as input
    CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;
}
bool CSupervisor::Query_Uniform_Compute_StorageImage(){ return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;}
void CSupervisor::Activate_Uniform_Compute_StorageImage_Swapchain(){ //as output
    CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;
}
bool CSupervisor::Query_Uniform_Compute_StorageImage_Swapchain(){ return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;}

void CSupervisor::Activate_Feature_Graphics_DepthTest(){ m_app->swapchain.EnableDepthTest();}  
void CSupervisor::Activate_Feature_Graphics_MSAA(){ 
    m_app->swapchain.EnableMSAA();
    Activate_Feature_Graphics_DepthTest();//If enable MSAA, must also enable Depth Test
}
void CSupervisor::Activate_Feature_Graphics_48BPT(){ b48bpt = true;}
void CSupervisor::Activate_Feature_Graphics_PushConstant(){ bPushConstant = true;}
void CSupervisor::Activate_Feature_Graphics_Blend(){ bBlend = true;}
void CSupervisor::Activate_Feature_Graphics_RainbowMipmap(){ bRainbowMipmap = true;}

void CSupervisor::Activate_Buffer_Graphics_Vertex(std::unique_ptr<std::vector<std::string>> modelNames, CModelManager &modelManager){ 
    for(int i = 0; i < modelNames->size(); i++){
        VertexStructureType = VertexStructureTypes::ThreeDimension;
        if((*modelNames)[i] == "CUSTOM3D0"){
            m_app->renderer.CreateVertexBuffer<Vertex3D>(modelManager.customModels3D[0].vertices); 
            m_app->renderer.CreateIndexBuffer(modelManager.customModels3D[0].indices);
            
            m_app->modelManager.modelLengths.push_back(modelManager.customModels3D[0].length);
            m_app->modelManager.modelLengthsMin.push_back(modelManager.customModels3D[0].lengthMin);
            m_app->modelManager.modelLengthsMax.push_back(modelManager.customModels3D[0].lengthMax);
        }else if((*modelNames)[i] == "CUSTOM2D0"){
            VertexStructureType = VertexStructureTypes::TwoDimension;
            m_app->renderer.CreateVertexBuffer<Vertex2D>(modelManager.customModels2D[0].vertices); 

            m_app->modelManager.modelLengths.push_back(modelManager.customModels2D[0].length);
            m_app->modelManager.modelLengthsMin.push_back(modelManager.customModels2D[0].lengthMin);
            m_app->modelManager.modelLengthsMax.push_back(modelManager.customModels2D[0].lengthMax);
        }else{
            VertexStructureType = VertexStructureTypes::ThreeDimension;
            std::vector<Vertex3D> modelVertices3D;
            std::vector<uint32_t> modelIndices3D;
            m_app->modelManager.LoadObjModel((*modelNames)[i], modelVertices3D, modelIndices3D);
            m_app->renderer.CreateVertexBuffer<Vertex3D>(modelVertices3D); 
            m_app->renderer.CreateIndexBuffer(modelIndices3D);
        }
    }
}          
void CSupervisor::Activate_Buffer_Graphics_Vertex(VertexStructureTypes vertexStructureType){ 
    VertexStructureType = vertexStructureType;
}  

void CSupervisor::Activate_Texture(std::unique_ptr<std::vector<TextureAttributeInfo>> textureAttributes){
    //Textures
    //if(Query_Pipeline_Graphics()){ //remove this query because if present texutre to swapchain, no need graphics pipeline
    if(textureAttributes){
        VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        //VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
        for(int i = 0; i < textureAttributes->size(); i++){
            //auto startTextureTime = std::chrono::high_resolution_clock::now();

            if((*textureAttributes)[i].miplevel > 1) //mipmap
                usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            else 
                if(Query_Uniform_Compute_StorageImage()) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            if(!b48bpt) //24bpt
                if(Query_Uniform_Compute_StorageImage_Swapchain()) m_app->textureManager.CreateTextureImage((*textureAttributes)[i].name, usage, m_app->renderer.commandPool, (*textureAttributes)[i].miplevel, (*textureAttributes)[i].samplerid, m_app->swapchain.swapChainImageFormat);
                else m_app->textureManager.CreateTextureImage((*textureAttributes)[i].name, usage, m_app->renderer.commandPool, (*textureAttributes)[i].miplevel, (*textureAttributes)[i].samplerid, VK_FORMAT_R8G8B8A8_SRGB, 8, (*textureAttributes)[i].enableCubemap);  
            else //48bpt
                m_app->textureManager.CreateTextureImage((*textureAttributes)[i].name, usage, m_app->renderer.commandPool, (*textureAttributes)[i].miplevel, (*textureAttributes)[i].samplerid, VK_FORMAT_R16G16B16A16_UNORM, 16, (*textureAttributes)[i].enableCubemap); 
            
            if(bRainbowMipmap){
                VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                if((*textureAttributes)[i].miplevel > 1) m_app->textureManager.textureImages[i].generateMipmaps("checkerboard", usage_mipmap);
            }else if((*textureAttributes)[i].miplevel > 1) m_app->textureManager.textureImages[i].generateMipmaps();
            
            //auto endTextureTime = std::chrono::high_resolution_clock::now();
            //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
            //std::cout<<"Load Texture '"<< (*textureNames)[i].first <<"' cost: "<<durationTime<<" milliseconds"<<std::endl;
        }

    }
    //}
}

void CSupervisor::Activate_Pipeline(){ //*customBinding = NULL
    //std::cout<<"Activate_Pipeline()"<<std::endl;

    //Command buffers
    //m_app->renderer.CreateCommandPool(m_app->surface);
    if(Query_Pipeline_Graphics()) m_app->renderer.CreateGraphicsCommandBuffer();
    if(Query_Pipeline_Compute()) m_app->renderer.CreateComputeCommandBuffer();

    //Framebuffers
    if(Query_Pipeline_Graphics()){
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        //m_app->renderProcess.addColorAttachment(m_app->swapchain.swapChainImageFormat);
        m_app->renderProcess.addColorAttachment( 
            m_app->swapchain.swapChainImageFormat,  
            m_app->swapchain.bEnableDepthTest,  
            m_app->swapchain.depthFormat,  
            m_app->swapchain.msaaSamples, 
            imageLayout); //add this function will enable color attachment (bUseColorAttachment = true)
        m_app->renderProcess.createSubpass();
        if(m_app->swapchain.bEnableDepthTest){
            VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            m_app->renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
        }else m_app->renderProcess.createDependency();
        m_app->renderProcess.createRenderPass();

        if(bBlend)
            m_app->renderProcess.addColorBlendAttachment(
                VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);

        m_app->swapchain.CreateFramebuffers(m_app->renderProcess.renderPass);
    }

    //Shaders
    if(Query_Pipeline_Graphics()){
        for(int i = 0; i < m_app->appInfo.Object.Pipeline.VertexShader->size(); i++){
            m_app->shaderManager.CreateShader((*m_app->appInfo.Object.Pipeline.VertexShader)[i], m_app->shaderManager.VERT);
            m_app->shaderManager.CreateShader((*m_app->appInfo.Object.Pipeline.FragmentShader)[i], m_app->shaderManager.FRAG);
        }
        //m_app->shaderManager.CreateShader(VertexShader, m_app->shaderManager.VERT);
        //m_app->shaderManager.CreateShader(FragmentShader, m_app->shaderManager.FRAG);
    }
    if(Query_Pipeline_Compute())
        for(int i = 0; i < m_app->appInfo.Object.Pipeline.ComputeShader->size(); i++)
            m_app->shaderManager.CreateShader((*m_app->appInfo.Object.Pipeline.ComputeShader)[i], m_app->shaderManager.COMP);
        //m_app->shaderManager.CreateShader(ComputeShader, m_app->shaderManager.COMP);

    //Push constant
    if(Query_Pipeline_Graphics())
        if(bPushConstant) 
            m_app->shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);

    //std::cout<<"before uniform()"<<std::endl;
    //Uniforms
    if(Query_Pipeline_Graphics()){
        if(Query_Uniform_Graphics_VP()) CGraphicsDescriptorManager::addVPUniformBuffer();
        if(Query_Uniform_Graphics_MVP()) CGraphicsDescriptorManager::addMVPUniformBuffer();//make MVP default for all sample. even some sample doesn't need
        //uint32_t mipLevels = m_app->textureManager.textureImages[0].mipLevels;//use the miplevels from the first texture image
        //TODO: Currently each texture can choose to compute mipmaps or not. 
        //But the sampler is using first miplevels for all textures
        //Need make change so only mipmaped texture use mipmaped sampler
        //std::cout<<"before sampler()"<<std::endl;
        if(Query_Uniform_Graphics_Sampler())
            CGraphicsDescriptorManager::addImageSamplerUniformBuffer(m_app->appInfo.Uniform.SamplerMiplevels);
            //for(int i = 0; i < m_samplerCount; i++)
                //CGraphicsDescriptorManager::addImageSamplerUniformBuffer(m_app->textureManager.textureImages[i].mipLevels);//TODO: mipLevels is per texture image; but there are multiple pipelines, which texture sampler to use?
        //std::cout<<"before custom()"<<std::endl;
        if(Query_Uniform_Graphics_Custom()) CGraphicsDescriptorManager::addCustomUniformBuffer(GraphicsCustomUniformBufferSize);
        //std::cout<<"after custom()"<<std::endl;
    }
    if(Query_Pipeline_Compute()){
        if(Query_Uniform_Compute_StorageBuffer()) {
            //VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
            CComputeDescriptorManager::addStorageBuffer(ComputeStorageBufferSize, ComputeStorageBufferUsage);
        }
        if(Query_Uniform_Compute_StorageImage()) CComputeDescriptorManager::addStorageImage(UNIFORM_IMAGE_STORAGE_TEXTURE_BIT);
        if(Query_Uniform_Compute_StorageImage_Swapchain()) CComputeDescriptorManager::addStorageImage(UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT);
        if(Query_Uniform_Compute_Custom()) CComputeDescriptorManager::addCustomUniformBuffer(ComputeCustomUniformBufferSize);
    }

    //std::cout<<"before pool()"<<std::endl;
    //Descriptor Pool
    //CDescriptorManager::createDescriptorPool(m_app->appInfo.Object.Count); 
    CDescriptorManager::createDescriptorPool(m_app->objectList.size()); 

    //std::cout<<"after pool()"<<std::endl;

    //Descriptor Layout
    if(Query_Pipeline_Graphics()){
        if(Query_Uniform_Graphics_Custom()) {
            CGraphicsDescriptorManager::createDescriptorSetLayout(&GraphicsCustomBinding); 
        }else CGraphicsDescriptorManager::createDescriptorSetLayout();
        if(Query_Uniform_Graphics_Sampler())CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); 
    }
    if(Query_Pipeline_Compute()){
        if(Query_Uniform_Compute_Custom()){
            CComputeDescriptorManager::createDescriptorSetLayout(&ComputeCustomBinding);
        }else CComputeDescriptorManager::createDescriptorSetLayout();
    }

    //Descriptor Set
    if(Query_Pipeline_Graphics()){
        //if(Query_Uniform_Compute_StorageImage())
        //    m_app->graphicsDescriptorManager.createDescriptorSets(&(m_app->textureManager.textureImages));
        //else 
        m_app->graphicsDescriptorManager.createDescriptorSets();
    }
    if(Query_Pipeline_Compute()){
        if(Query_Uniform_Compute_StorageImage_Swapchain()) {
            if(Query_Uniform_Compute_StorageImage())
                m_app->computeDescriptorManager.createDescriptorSets(&(m_app->textureManager.textureImages), &(m_app->swapchain.views));
            else m_app->computeDescriptorManager.createDescriptorSets(NULL, &(m_app->swapchain.views));
        }else m_app->computeDescriptorManager.createDescriptorSets();
    }

    //Pipeline
    if(Query_Pipeline_Graphics()){
        std::vector<VkDescriptorSetLayout> dsLayouts;
        dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
        if(Query_Uniform_Graphics_Sampler()) dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

        //!!!Different cube can share the same texture descriptor.
        //suppose we have 100 objects, 100 different textures. cube x 50, sphere x 50. How many texture layouts? How many texture descriptor?
        //obviously, every objects need a different texture, so bind with objectId
        //but for layout, can use one. That means texture layout should be object property, while the descriptor set(associate with image) should be cube[i]/sphere[i] bound

        //each object can have muti texture image, multi descriptor set(when creating descritpor set, need a sampler)
        //all objects share the same descriptor pool and descriptor layout, they are universal
        //sampler should also be universal
        
        //std::cout<<"Begin create graphics pipeline"<<std::endl;
        for(int i = 0; i < m_app->appInfo.Object.Pipeline.VertexShader->size(); i++){
            //! All graphics pipelines use the same dsLayouts
            if(bPushConstant)  m_app->renderProcess.createGraphicsPipelineLayout(dsLayouts,  m_app->shaderManager.pushConstantRange, true, i);
            else m_app->renderProcess.createGraphicsPipelineLayout(dsLayouts, i);

            //std::cout<<"create graphics pipeline: "<<i<<std::endl;
            switch(VertexStructureType){
                case VertexStructureTypes::NoType:
                    m_app->renderProcess.createGraphicsPipeline(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        m_app->shaderManager.vertShaderModules[i], 
                        m_app->shaderManager.fragShaderModules[i], i);  
                break;
                case VertexStructureTypes::ThreeDimension:
                    m_app->renderProcess.createGraphicsPipeline<Vertex3D>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        m_app->shaderManager.vertShaderModules[i], 
                        m_app->shaderManager.fragShaderModules[i], true, i);  
                break;
                case VertexStructureTypes::TwoDimension:
                    m_app->renderProcess.createGraphicsPipeline<Vertex2D>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        m_app->shaderManager.vertShaderModules[i], 
                        m_app->shaderManager.fragShaderModules[i], true, i);
                break;
                case VertexStructureTypes::ParticleType:
                    m_app->renderProcess.createGraphicsPipeline<Particle>(
                        VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 
                        m_app->shaderManager.vertShaderModules[i], 
                        m_app->shaderManager.fragShaderModules[i], true, i);  
                break;
                default:
                break;
            }
        }
        //std::cout<<"Done create graphics pipeline"<<std::endl;
    }
    if(Query_Pipeline_Compute()){ //for now assume only one compute pipeline
        //! only support one compute pipeline
        //std::cout<<"before pipeline layout(compute)"<<std::endl;
        m_app->renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
        //std::cout<<"after pipeline layout(compute)"<<std::endl;
        m_app->renderProcess.createComputePipeline(m_app->shaderManager.compShaderModules[0]);
        //std::cout<<"after pipeline(compute)"<<std::endl;
    }
}


void CSupervisor::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    dsSets.push_back(m_app->computeDescriptorManager.descriptorSets);

    m_app->renderer.BindComputeDescriptorSets(m_app->renderProcess.computePipelineLayout, dsSets, -1); //-1 to offset means no dynamic offset

    //std::cout<<"Record Compute command buffer. "<<std::endl;
    m_app->renderer.Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
}    