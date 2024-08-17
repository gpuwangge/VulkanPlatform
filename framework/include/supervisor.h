#ifndef H_SUPERVISOR
#define H_SUPERVISOR
// #include "common.h"
// #include "context.h"
// #include "dataBuffer.hpp"
// #include "descriptor.h"
#include "application.h"

/***************
There is no such thing as a static class in C++. 
The closest approximation is a class that only contains static data members and static methods.
Static data members in a class are shared by all the class objects as there is only one copy of them in the memory, 
regardless of the number of objects of the class. Static methods in a class can only access static data members, 
other static methods or any methods outside the class.
*****************/

class CSupervisor {
public:
    CSupervisor(){}
    static CApplication *m_app;
    static void Register(CApplication *app){ m_app = app; m_app->renderer.CreateCommandPool(m_app->surface);} //need command pool for texture creation

    /*****************************
     * 
     * Shaders
     * 
     *****************************/
    static std::string VertexShader;
    static std::string FragmentShader;
    static std::string ComputeShader;
    static bool Query_Pipeline_Graphics(){ return VertexShader!="";}
    static bool Query_Pipeline_Compute(){ return ComputeShader!="";}

    /*****************************
     * 
     * Graphics Vectors
     * 
     *****************************/
    static std::vector<Vertex2D> vertices2D;
    static std::vector<Vertex3D> vertices3D;
    static std::vector<uint32_t> indices3D;

    /*****************************
     * 
     * Graphcis Uniform
     * 
     *****************************/
    static VkDeviceSize GraphicsCustomUniformBufferSize;
    static VkDescriptorSetLayoutBinding GraphicsCustomBinding; 
    static int m_samplerCount;

    static void Activate_Uniform_Graphics_VP(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_VP_BIT; }
    static bool Query_Uniform_Graphics_VP(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_VP_BIT;}

    static void Activate_Uniform_Graphics_MVP(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_MVP_BIT;}
    static bool Query_Uniform_Graphics_MVP(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_MVP_BIT;}

    static void Activate_Uniform_Graphics_Custom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding){ 
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT;
        GraphicsCustomUniformBufferSize = graphicsCustomUniformBufferSize;
        GraphicsCustomBinding = graphicsCustomBinding;
        }
    static bool Query_Uniform_Graphics_Custom(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT;}
    
    static void Activate_Uniform_Graphics_Sampler(int samplerCount = 1){ m_samplerCount = samplerCount; CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_SAMPLER_BIT;}
    static bool Query_Uniform_Graphics_Sampler(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT;}

    /*****************************
     * 
     * Compute Uniform
     * 
     *****************************/
    static VkDeviceSize ComputeCustomUniformBufferSize;
    static VkDescriptorSetLayoutBinding ComputeCustomBinding;     
    static VkDeviceSize ComputeStorageBufferSize;
    static VkBufferUsageFlags ComputeStorageBufferUsage;

    static void Activate_Uniform_Compute_Custom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding){ 
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;
        ComputeCustomUniformBufferSize = graphicsCustomUniformBufferSize;
        ComputeCustomBinding = graphicsCustomBinding; 
    }
    static bool Query_Uniform_Compute_Custom(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;}

    static void Activate_Uniform_Compute_StorageBuffer(VkDeviceSize computeStorageBufferSize, VkBufferUsageFlags storageBufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT){ 
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_STORAGE_BIT;
        ComputeStorageBufferSize = computeStorageBufferSize;
        ComputeStorageBufferUsage = storageBufferUsage;
    }
    static bool Query_Uniform_Compute_StorageBuffer(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_BIT;}

    static void Activate_Uniform_Compute_StorageImage(){  //as input
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;
    }
    static bool Query_Uniform_Compute_StorageImage(){ return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;}

    static void Activate_Uniform_Compute_StorageImage_Swapchain(){ //as output
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;
    }
    static bool Query_Uniform_Compute_StorageImage_Swapchain(){ return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;}


    /*****************************
     * 
     * Graphcis Feature
     * 
     *****************************/
    static bool b48bpt;
    static bool bPushConstant;
    static bool bBlend;
    static bool bRainbowMipmap;
    static void Activate_Feature_Graphics_DepthTest(){ m_app->swapchain.EnableDepthTest();}  
    static void Activate_Feature_Graphics_MSAA(){ 
        m_app->swapchain.EnableMSAA();
        Activate_Feature_Graphics_DepthTest();//If enable MSAA, must also enable Depth Test
    }
    static void Activate_Feature_Graphics_48BPT(){ b48bpt = true;}
    static void Activate_Feature_Graphics_PushConstant(){ bPushConstant = true;}
    static void Activate_Feature_Graphics_Blend(){ bBlend = true;}
    static void Activate_Feature_Graphics_RainbowMipmap(){ bRainbowMipmap = true;}

    /*****************************
     * 
     * Graphcis Buffer
     * 
     *****************************/    
    static VertexStructureTypes VertexStructureType;
    static void Activate_Buffer_Graphics_Vertex(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D){ 
        VertexStructureType = VertexStructureTypes::ThreeDimension;
        m_app->renderer.CreateVertexBuffer<Vertex3D>(vertices3D); 
        m_app->renderer.CreateIndexBuffer(indices3D);
    }
    static void Activate_Buffer_Graphics_Vertex(std::vector<Vertex2D> &vertices2D){ 
        VertexStructureType = VertexStructureTypes::TwoDimension;
        m_app->renderer.CreateVertexBuffer<Vertex2D>(vertices2D); 
        //m_app->renderer.CreateIndexBuffer(indices3D);
    }
    static void Activate_Buffer_Graphics_Vertex(std::vector<std::string> &modelNames){ 
        VertexStructureType = VertexStructureTypes::ThreeDimension;
        for(int i = 0; i < modelNames.size(); i++){
            m_app->modelManager.LoadObjModel(modelNames[i], vertices3D, indices3D);
            m_app->renderer.CreateVertexBuffer<Vertex3D>(vertices3D); 
            m_app->renderer.CreateIndexBuffer(indices3D);
        }
    }        
    static void Activate_Buffer_Graphics_Vertex(VertexStructureTypes vertexStructureType){ 
        VertexStructureType = vertexStructureType;
    }     

    /*****************************
     * 
     * Texture
     * 
     *****************************/
    static void Activate_Texture(std::vector<std::pair<std::string, bool>> *textureNames = NULL){
        //Textures
        //if(Query_Pipeline_Graphics()){ //remove this query because if present texutre to swapchain, no need graphics pipeline
        if(textureNames){
            VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            //VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
            for(int i = 0; i < textureNames->size(); i++){
                if((*textureNames)[i].second) //mipmap
                    usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                else 
                    if(Query_Uniform_Compute_StorageImage()) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                    else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                if(!b48bpt) //24bpt
                    if(Query_Uniform_Compute_StorageImage_Swapchain()) m_app->textureManager.CreateTextureImage((*textureNames)[i].first, usage, m_app->renderer.commandPool, (*textureNames)[i].second, m_app->swapchain.swapChainImageFormat);
                    else m_app->textureManager.CreateTextureImage((*textureNames)[i].first, usage, m_app->renderer.commandPool, (*textureNames)[i].second);  
                else //48bpt
                    m_app->textureManager.CreateTextureImage((*textureNames)[i].first, usage, m_app->renderer.commandPool, (*textureNames)[i].second, VK_FORMAT_R16G16B16A16_UNORM, 16); 
                
                if(bRainbowMipmap){
                    VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    if((*textureNames)[i].second) m_app->textureManager.textureImages[i].generateMipmaps("checkerboard", usage_mipmap);
                }else if((*textureNames)[i].second) m_app->textureManager.textureImages[i].generateMipmaps();
            }
            }
       //}
    }

    /*****************************
     * 
     * Pipeline
     * 
     *****************************/
    static void Activate_Pipeline(){ //*customBinding = NULL
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
		    m_app->shaderManager.CreateShader(VertexShader, m_app->shaderManager.VERT);
		    m_app->shaderManager.CreateShader(FragmentShader, m_app->shaderManager.FRAG);
        }
        if(Query_Pipeline_Compute())
            m_app->shaderManager.CreateShader(ComputeShader, m_app->shaderManager.COMP);

        //Push constant
        if(Query_Pipeline_Graphics())
            if(bPushConstant) 
                m_app->shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);

        //Uniforms
        if(Query_Pipeline_Graphics()){
            if(Query_Uniform_Graphics_VP()) CGraphicsDescriptorManager::addVPUniformBuffer();
            if(Query_Uniform_Graphics_MVP()) CGraphicsDescriptorManager::addMVPUniformBuffer();//make MVP default for all sample. even some sample doesn't need
            //uint32_t mipLevels = m_app->textureManager.textureImages[0].mipLevels;//use the miplevels from the first texture image
            //TODO: Currently each texture can choose to compute mipmaps or not. 
            //But the sampler is using first miplevels for all textures
            //Need make change so only mipmaped texture use mipmaped sampler
            if(Query_Uniform_Graphics_Sampler())
                for(int i = 0;i < m_samplerCount; i++)
                    CGraphicsDescriptorManager::addImageSamplerUniformBuffer(m_app->textureManager.textureImages[i].mipLevels);
            if(Query_Uniform_Graphics_Custom()) CGraphicsDescriptorManager::addCustomUniformBuffer(GraphicsCustomUniformBufferSize);
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

        //Pool
		CDescriptorManager::createDescriptorPool(); 

        //Layout
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

        //Set
        if(Query_Pipeline_Graphics()){
		    if(Query_Uniform_Compute_StorageImage())
                m_app->graphicsDescriptorManager.createDescriptorSets(&(m_app->textureManager.textureImages));
            else m_app->graphicsDescriptorManager.createDescriptorSets();
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

            
            if(bPushConstant) 
                m_app->renderProcess.createGraphicsPipelineLayout(dsLayouts,  m_app->shaderManager.pushConstantRange);
            else
                m_app->renderProcess.createGraphicsPipelineLayout(dsLayouts);
            
            switch(VertexStructureType){
                case VertexStructureTypes::NoType:
                    m_app->renderProcess.createGraphicsPipeline(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        m_app->shaderManager.vertShaderModule, 
                        m_app->shaderManager.fragShaderModule);  
                break;
                case VertexStructureTypes::ThreeDimension:
                    m_app->renderProcess.createGraphicsPipeline<Vertex3D>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        m_app->shaderManager.vertShaderModule, 
                        m_app->shaderManager.fragShaderModule);  
                break;
                case VertexStructureTypes::TwoDimension:
                    m_app->renderProcess.createGraphicsPipeline<Vertex2D>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        m_app->shaderManager.vertShaderModule, 
                        m_app->shaderManager.fragShaderModule);
                break;
                case VertexStructureTypes::ParticleType:
                    m_app->renderProcess.createGraphicsPipeline<Particle>(
                        VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 
                        m_app->shaderManager.vertShaderModule, 
                        m_app->shaderManager.fragShaderModule);  
                break;
                default:
                break;
            }

        }
        if(Query_Pipeline_Compute()){
            m_app->renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
		    m_app->renderProcess.createComputePipeline(m_app->shaderManager.compShaderModule);
        }
    }

    static void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
        std::vector<std::vector<VkDescriptorSet>> dsSets; 
		dsSets.push_back(m_app->computeDescriptorManager.descriptorSets);

		m_app->renderer.BindComputeDescriptorSets(m_app->renderProcess.computePipelineLayout, dsSets, -1); //-1 to offset means no dynamic offset

		//std::cout<<"Record Compute command buffer. "<<std::endl;
		m_app->renderer.Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
    }
};

//Declare static variables here:
CApplication *CSupervisor::m_app;
std::string CSupervisor::VertexShader;
std::string CSupervisor::FragmentShader;
std::string CSupervisor::ComputeShader;
std::vector<Vertex2D> CSupervisor::vertices2D;
std::vector<Vertex3D> CSupervisor::vertices3D;
std::vector<uint32_t> CSupervisor::indices3D;
VkDeviceSize CSupervisor::GraphicsCustomUniformBufferSize;
VkDescriptorSetLayoutBinding CSupervisor::GraphicsCustomBinding;
VkDeviceSize CSupervisor::ComputeStorageBufferSize;
VkBufferUsageFlags CSupervisor::ComputeStorageBufferUsage;
VkDeviceSize CSupervisor::ComputeCustomUniformBufferSize;
VkDescriptorSetLayoutBinding CSupervisor::ComputeCustomBinding;
int CSupervisor::m_samplerCount;
//bool CMastermind::bDepthTest;
//bool CMastermind::bMSAA;
bool CSupervisor::b48bpt;
bool CSupervisor::bPushConstant;
bool CSupervisor::bBlend;
VertexStructureTypes CSupervisor::VertexStructureType;
bool CSupervisor::bRainbowMipmap;

// class CMastermind { //compute helper
// public:
//    CMastermind(){

//    }
// };

#endif