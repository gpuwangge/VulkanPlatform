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

class CSkyvision { //graphics helper
public:
    static CApplication *m_app;

    static std::vector<Vertex2D> vertices2D;
    static std::vector<Vertex3D> vertices3D;
    static std::vector<uint32_t> indices3D;

    static bool b48bpt;
    static bool bPushConstant;
    static bool bVP;

    CSkyvision(){}
    static void Register(CApplication *app){ m_app = app;}
    static void ActivateDepthTest(){ m_app->swapchain.EnableDepthTest();}
    static void ActivateMSAA(){ m_app->swapchain.EnableMSAA();}
    static void Activate48BPT(){ b48bpt = true;}
    static void ActivatePushConstant(){ bPushConstant = true;}
    static void ActivateVP(){ bVP = true;}

    //Load 3D mesh resource from model files
    static void LoadResources(std::vector<std::string> &modelNames, std::vector<std::pair<std::string, bool>> &textureNames, 
            std::string vertexShader, std::string fragmentShader, 
            int numSampler = 1,
            VkDeviceSize customUniformBufferSize = 0, VkDescriptorSetLayoutBinding customBinding = {0}){
        LoadResources(textureNames, vertexShader, fragmentShader, numSampler, customUniformBufferSize, customBinding);
        for(int i = 0; i < modelNames.size(); i++){
            m_app->modelManager.LoadObjModel(modelNames[i], vertices3D, indices3D);
            m_app->renderer.CreateVertexBuffer<Vertex3D>(vertices3D); 
            m_app->renderer.CreateIndexBuffer(indices3D);
        }
    }

    //Load 3D mesh resource from vertex buffer
    static void LoadResources(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, std::vector<std::pair<std::string, bool>> &textureNames, 
            std::string vertexShader, std::string fragmentShader,
            int numSampler = 1,
            VkDeviceSize customUniformBufferSize = 0, VkDescriptorSetLayoutBinding customBinding = {0}){
        LoadResources(textureNames, vertexShader, fragmentShader, numSampler, customUniformBufferSize, customBinding);
        m_app->renderer.CreateVertexBuffer<Vertex3D>(vertices3D); 
        m_app->renderer.CreateIndexBuffer(indices3D);
    }

    //Load 2D mesh resource from vertex buffer
    static void LoadResources(std::vector<Vertex2D> &vertices2D, std::vector<std::pair<std::string, bool>> &textureNames, 
            std::string vertexShader, std::string fragmentShader,
            int numSampler = 1,
            VkDeviceSize customUniformBufferSize = 0, VkDescriptorSetLayoutBinding customBinding = {0}){
        LoadResources(textureNames, vertexShader, fragmentShader, numSampler, customUniformBufferSize, customBinding);
        m_app->renderer.CreateVertexBuffer<Vertex2D>(vertices2D); 
        //m_app->renderer.CreateIndexBuffer(indices3D);
    }

    //Load textures and shaders
    static void LoadResources(std::vector<std::pair<std::string, bool>> &textureNames, 
            std::string vertexShader, std::string fragmentShader,
            int numSampler = 1,
            VkDeviceSize customUniformBufferSize = 0, VkDescriptorSetLayoutBinding customBinding = {0}){ //*customBinding = NULL
        //Command buffers
        m_app->renderer.CreateCommandPool(m_app->surface);
		m_app->renderer.CreateGraphicsCommandBuffer();


        //Textures
        VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        for(int i = 0; i < textureNames.size(); i++){
            if(textureNames[i].second) //mipmap
                usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            else 
                usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		    if(!b48bpt) //24bpt
                m_app->textureManager.CreateTextureImage(textureNames[i].first, usage, m_app->renderer.commandPool, textureNames[i].second); 
            else //48bpt
                m_app->textureManager.CreateTextureImage(textureNames[i].first, usage, m_app->renderer.commandPool, textureNames[i].second, VK_FORMAT_R16G16B16A16_UNORM, 16); 
            
            if(textureNames[i].second) m_app->textureManager.textureImages[i].generateMipmaps();

            //To create rainbow mipmaps
            //VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            //if(textureNames[i].second) m_app->textureManager.textureImages[i].generateMipmaps("checkerboard", usage_mipmap);
        }

        //Framebuffers
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


        //Shaders
		m_app->shaderManager.CreateShader(vertexShader, m_app->shaderManager.VERT);
		m_app->shaderManager.CreateShader(fragmentShader, m_app->shaderManager.FRAG);

        //Push constant
        if(bPushConstant) m_app->shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);

            

        //Uniforms
        if(bVP) CGraphicsDescriptorManager::addVPUniformBuffer();
        else CGraphicsDescriptorManager::addMVPUniformBuffer();//make MVP default for all sample. even some sample doesn't need
		//uint32_t mipLevels = m_app->textureManager.textureImages[0].mipLevels;//use the miplevels from the first texture image
        //TODO: Currently each texture can choose to compute mipmaps or not. 
        //But the sampler is using first miplevels for all textures
        //Need make change so only mipmaped texture use mipmaped sampler
        for(int i = 0;i < numSampler; i++)
		    CGraphicsDescriptorManager::addImageSamplerUniformBuffer(m_app->textureManager.textureImages[i].mipLevels);
       
		if(customUniformBufferSize>0) CGraphicsDescriptorManager::addCustomUniformBuffer(customUniformBufferSize);
		CDescriptorManager::createDescriptorPool(); //-------------------------------------------------------------
        if(customUniformBufferSize>0) {
		    CGraphicsDescriptorManager::createDescriptorSetLayout(&customBinding); 
        }else CGraphicsDescriptorManager::createDescriptorSetLayout();
		CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); //layout size = 1
        //--------------------------------------------------------------------------------------------------
		m_app->graphicsDescriptorManager.createDescriptorSets();
    }


};

//Declare static variables here:
CApplication *CSkyvision::m_app;
std::vector<Vertex2D> CSkyvision::vertices2D;
std::vector<Vertex3D> CSkyvision::vertices3D;
std::vector<uint32_t> CSkyvision::indices3D;
bool CSkyvision::b48bpt;
bool CSkyvision::bPushConstant;
bool CSkyvision::bVP;

class CMastermind { //compute helper
public:
   CMastermind(){

   }
};

#endif