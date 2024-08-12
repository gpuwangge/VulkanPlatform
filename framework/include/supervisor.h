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

class CMastermind { //graphics helper
public:
    CMastermind(){}
    static CApplication *m_app;
    static void Register(CApplication *app){ m_app = app;}

    //Shaders
    static std::string VertexShader;
    static std::string FragmentShader;
    static std::string ComputeShader;
    static bool QueryGraphicsPipeline(){ return VertexShader!="";}
    static bool QueryComputePipeline(){ return ComputeShader!="";}

    //Graphics Vectors
    static std::vector<Vertex2D> vertices2D;
    static std::vector<Vertex3D> vertices3D;
    static std::vector<uint32_t> indices3D;

    //Graphcis Uniform
    static VkDeviceSize GraphicsCustomUniformBufferSize;
    static VkDescriptorSetLayoutBinding GraphicsCustomBinding; //{0}
    static void ActivateVP(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_VP_BIT; }
    static void ActivateMVP(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_MVP_BIT;}
    static void ActivateGraphcisCustom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding){ 
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT;
        GraphicsCustomUniformBufferSize = graphicsCustomUniformBufferSize;
        GraphicsCustomBinding = graphicsCustomBinding;
        }
    static int m_samplerCount;
    static void ActivateSampler(int samplerCount = 1){ m_samplerCount = samplerCount; CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_SAMPLER_BIT;}
    static bool QueryVP(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_VP_BIT;}
    static bool QueryMVP(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_MVP_BIT;}
    static bool QueryGraphicsCustom(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT;}
    static bool QuerySampler(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT;}

    //Compute Uniform
    static VkDeviceSize ComputeStorageBufferSize;
    static void ActivateComputeCustom(){ CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;}
    static void ActivateStorageBuffer(VkDeviceSize computeStorageBufferSize){ 
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_STORAGE_BIT;
        ComputeStorageBufferSize = computeStorageBufferSize;
        }
    static void ActivateStorageImage(bool bPresentSwapchain){ 
        if(bPresentSwapchain) CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;
        else CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;
    }
    static bool QueryComputeCustom(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;}
    static bool QueryStorageBuffer(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_BIT;}
    static bool QueryStorageImage(bool bPresentSwapchain){
        if(bPresentSwapchain) return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;
        return CDescriptorManager::uniformBufferUsageFlags | UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;
    }

    //Graphcis Feature
    static bool bDepthTest;
    static bool bMSAA;
    static bool b48bpt;
    static bool bPushConstant;
    static void ActivateDepthTest(){ bDepthTest = true; m_app->swapchain.EnableDepthTest();}
    static void ActivateMSAA(){ 
        bMSAA = true; m_app->swapchain.EnableMSAA();
        ActivateDepthTest();//If enable MSAA, must also enable Depth Test
    }
    static void Activate48BPT(){ b48bpt = true;}
    static void ActivatePushConstant(){ bPushConstant = true;}
    

    //Load 3D mesh resource from model files
    static void LoadResources(std::vector<std::string> &modelNames,  
            std::vector<std::pair<std::string, bool>> *textureNames = NULL){
        LoadResources(textureNames);
        for(int i = 0; i < modelNames.size(); i++){
            m_app->modelManager.LoadObjModel(modelNames[i], vertices3D, indices3D);
            m_app->renderer.CreateVertexBuffer<Vertex3D>(vertices3D); 
            m_app->renderer.CreateIndexBuffer(indices3D);
        }
    }

    //Load 3D mesh resource from vertex buffer
    static void LoadResources(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, 
            std::vector<std::pair<std::string, bool>> *textureNames = NULL){
        LoadResources(textureNames);
        m_app->renderer.CreateVertexBuffer<Vertex3D>(vertices3D); 
        m_app->renderer.CreateIndexBuffer(indices3D);
    }

    //Load 2D mesh resource from vertex buffer
    static void LoadResources(std::vector<Vertex2D> &vertices2D, 
            std::vector<std::pair<std::string, bool>> *textureNames = NULL){
        LoadResources(textureNames);
        m_app->renderer.CreateVertexBuffer<Vertex2D>(vertices2D); 
        //m_app->renderer.CreateIndexBuffer(indices3D);
    }

    //No load mesh model
    static void LoadResources(std::vector<std::pair<std::string, bool>> *textureNames = NULL){ //*customBinding = NULL
        //Command buffers
        m_app->renderer.CreateCommandPool(m_app->surface);
        if(QueryGraphicsPipeline()) m_app->renderer.CreateGraphicsCommandBuffer();
        if(QueryComputePipeline()) m_app->renderer.CreateComputeCommandBuffer();
        
        //Textures
        if(QueryGraphicsPipeline()){
            if(textureNames){
                VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                for(int i = 0; i < textureNames->size(); i++){
                    if((*textureNames)[i].second) //mipmap
                        usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    else 
                        usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    if(!b48bpt) //24bpt
                        m_app->textureManager.CreateTextureImage((*textureNames)[i].first, usage, m_app->renderer.commandPool, (*textureNames)[i].second); 
                    else //48bpt
                        m_app->textureManager.CreateTextureImage((*textureNames)[i].first, usage, m_app->renderer.commandPool, (*textureNames)[i].second, VK_FORMAT_R16G16B16A16_UNORM, 16); 
                    
                    if((*textureNames)[i].second) m_app->textureManager.textureImages[i].generateMipmaps();

                    //To create rainbow mipmaps
                    //VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    //if(textureNames[i].second) m_app->textureManager.textureImages[i].generateMipmaps("checkerboard", usage_mipmap);
                }
            }
        }

        //Framebuffers
        if(QueryGraphicsPipeline()){
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

            m_app->swapchain.CreateFramebuffers(m_app->renderProcess.renderPass);
        }

        //Shaders
        if(QueryGraphicsPipeline()){
		    m_app->shaderManager.CreateShader(VertexShader, m_app->shaderManager.VERT);
		    m_app->shaderManager.CreateShader(FragmentShader, m_app->shaderManager.FRAG);
        }
        if(QueryComputePipeline())
            m_app->shaderManager.CreateShader(ComputeShader, m_app->shaderManager.COMP);

        //Push constant
        if(QueryGraphicsPipeline())
            if(bPushConstant) 
                m_app->shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);

        //Uniforms
        if(QueryGraphicsPipeline()){
            if(QueryVP()) CGraphicsDescriptorManager::addVPUniformBuffer();
            if(QueryMVP()) CGraphicsDescriptorManager::addMVPUniformBuffer();//make MVP default for all sample. even some sample doesn't need
            //uint32_t mipLevels = m_app->textureManager.textureImages[0].mipLevels;//use the miplevels from the first texture image
            //TODO: Currently each texture can choose to compute mipmaps or not. 
            //But the sampler is using first miplevels for all textures
            //Need make change so only mipmaped texture use mipmaped sampler
            if(QuerySampler())
                for(int i = 0;i < m_samplerCount; i++)
                    CGraphicsDescriptorManager::addImageSamplerUniformBuffer(m_app->textureManager.textureImages[i].mipLevels);
            if(QueryGraphicsCustom()) CGraphicsDescriptorManager::addCustomUniformBuffer(GraphicsCustomUniformBufferSize);
        }
        if(QueryComputePipeline())
            if(QueryStorageBuffer())CComputeDescriptorManager::addStorageBuffer(ComputeStorageBufferSize);
		    
        //Pool
		CDescriptorManager::createDescriptorPool(); 

        //Layout
        if(QueryGraphicsPipeline()){
            if(QueryGraphicsCustom()) {
		        CGraphicsDescriptorManager::createDescriptorSetLayout(&GraphicsCustomBinding); 
            }else CGraphicsDescriptorManager::createDescriptorSetLayout();
            if(QuerySampler())CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); 
        }
        if(QueryComputePipeline())
            CComputeDescriptorManager::createDescriptorSetLayout();

        //Set
        if(QueryGraphicsPipeline())
		    m_app->graphicsDescriptorManager.createDescriptorSets();
        if(QueryComputePipeline())
            m_app->computeDescriptorManager.createDescriptorSets();
        
    }


};

//Declare static variables here:
CApplication *CMastermind::m_app;
std::string CMastermind::VertexShader;
std::string CMastermind::FragmentShader;
std::string CMastermind::ComputeShader;
std::vector<Vertex2D> CMastermind::vertices2D;
std::vector<Vertex3D> CMastermind::vertices3D;
std::vector<uint32_t> CMastermind::indices3D;
VkDeviceSize CMastermind::GraphicsCustomUniformBufferSize;
VkDescriptorSetLayoutBinding CMastermind::GraphicsCustomBinding;
VkDeviceSize CMastermind::ComputeStorageBufferSize;
int CMastermind::m_samplerCount;
bool CMastermind::bDepthTest;
bool CMastermind::bMSAA;
bool CMastermind::b48bpt;
bool CMastermind::bPushConstant;


// class CMastermind { //compute helper
// public:
//    CMastermind(){

//    }
// };

#endif