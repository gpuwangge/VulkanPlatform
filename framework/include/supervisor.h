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
    static VkDescriptorSetLayoutBinding GraphicsCustomBinding; 
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
    static VkDeviceSize ComputeCustomUniformBufferSize;
    static VkDescriptorSetLayoutBinding ComputeCustomBinding;     
    static VkDeviceSize ComputeStorageBufferSize;
    static VkBufferUsageFlags ComputeStorageBufferUsage;
    static void ActivateComputeCustom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding){ 
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;
        ComputeCustomUniformBufferSize = graphicsCustomUniformBufferSize;
        ComputeCustomBinding = graphicsCustomBinding; 
    }
    static void ActivateStorageBuffer(VkDeviceSize computeStorageBufferSize, VkBufferUsageFlags storageBufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT){ 
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_BUFFER_STORAGE_BIT;
        ComputeStorageBufferSize = computeStorageBufferSize;
        ComputeStorageBufferUsage = storageBufferUsage;
    }
    static void ActivateStorageImageTexture(){  //as input
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;
    }
    static void ActivateStorageImageSwapchain(){ //as output
        CDescriptorManager::uniformBufferUsageFlags |= UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;
    }
    static bool QueryComputeCustom(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT;}
    static bool QueryStorageBuffer(){return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_BIT;}
    static bool QueryStorageImageTexture(){ return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_IMAGE_STORAGE_TEXTURE_BIT;}
    static bool QueryStorageImageSwapchain(){ return CDescriptorManager::uniformBufferUsageFlags & UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT;}

    //Graphcis Feature
    //static bool bDepthTest;
    //static bool bMSAA;
    static bool b48bpt;
    static bool bPushConstant;
    static bool bBlend;
    static void ActivateDepthTest(){ m_app->swapchain.EnableDepthTest();} //bDepthTest = true; 
    static void ActivateMSAA(){ 
        //bMSAA = true; 
        m_app->swapchain.EnableMSAA();
        ActivateDepthTest();//If enable MSAA, must also enable Depth Test
    }
    static void Activate48BPT(){ b48bpt = true;}
    static void ActivatePushConstant(){ bPushConstant = true;}
    static void ActivateBlend(){ bBlend = true;}
    static VertexStructureTypes VertexStructureType;
    static void ActivateVertexBuffer(VertexStructureTypes vertexStructureType){ VertexStructureType = vertexStructureType;}

    //Load 3D mesh resource from model files
    static void LoadResources(std::vector<std::string> &modelNames,  
            std::vector<std::pair<std::string, bool>> *textureNames = NULL){
        ActivateVertexBuffer(VertexStructureTypes::ThreeDimension); //Model use Vertex3D buffer
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
                //VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                for(int i = 0; i < textureNames->size(); i++){
                    if((*textureNames)[i].second) //mipmap
                        usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    else 
                        if(QueryStorageImageTexture()) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                        else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    if(!b48bpt) //24bpt
                        if(QueryStorageImageSwapchain()) m_app->textureManager.CreateTextureImage((*textureNames)[i].first, usage, m_app->renderer.commandPool, (*textureNames)[i].second, m_app->swapchain.swapChainImageFormat);
                        else m_app->textureManager.CreateTextureImage((*textureNames)[i].first, usage, m_app->renderer.commandPool, (*textureNames)[i].second);  
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

            if(bBlend)
                m_app->renderProcess.addColorBlendAttachment(
                    VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                    VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);

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
        if(QueryComputePipeline()){
            if(QueryStorageBuffer()) {
                //VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
                CComputeDescriptorManager::addStorageBuffer(ComputeStorageBufferSize, ComputeStorageBufferUsage);
            }
            if(QueryStorageImageTexture()) CComputeDescriptorManager::addStorageImage(UNIFORM_IMAGE_STORAGE_TEXTURE_BIT);
            if(QueryStorageImageSwapchain()) CComputeDescriptorManager::addStorageImage(UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT);
            if(QueryComputeCustom()) CComputeDescriptorManager::addCustomUniformBuffer(ComputeCustomUniformBufferSize);
        }

        //Pool
		CDescriptorManager::createDescriptorPool(); 

        //Layout
        if(QueryGraphicsPipeline()){
            if(QueryGraphicsCustom()) {
		        CGraphicsDescriptorManager::createDescriptorSetLayout(&GraphicsCustomBinding); 
            }else CGraphicsDescriptorManager::createDescriptorSetLayout();
            if(QuerySampler())CGraphicsDescriptorManager::createTextureDescriptorSetLayout(); 
        }
        if(QueryComputePipeline()){
            if(QueryComputeCustom()){
                CComputeDescriptorManager::createDescriptorSetLayout(&ComputeCustomBinding);
            }else CComputeDescriptorManager::createDescriptorSetLayout();
        }

        //Set
        if(QueryGraphicsPipeline()){
		    if(QueryStorageImageTexture())
                m_app->graphicsDescriptorManager.createDescriptorSets(&(m_app->textureManager.textureImages));
            else m_app->graphicsDescriptorManager.createDescriptorSets();
        }
        if(QueryComputePipeline()){
            if(QueryStorageImageSwapchain()) {
                if(QueryStorageImageTexture())
                    m_app->computeDescriptorManager.createDescriptorSets(&(m_app->textureManager.textureImages), &(m_app->swapchain.views));
                else m_app->computeDescriptorManager.createDescriptorSets(NULL, &(m_app->swapchain.views));
            }else m_app->computeDescriptorManager.createDescriptorSets();
        }

        //Pipeline
        if(QueryGraphicsPipeline()){
            std::vector<VkDescriptorSetLayout> dsLayouts;
            dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout);
            if(QuerySampler()) dsLayouts.push_back(CGraphicsDescriptorManager::textureDescriptorSetLayout); //set = 1

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
        if(QueryComputePipeline()){
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
VkBufferUsageFlags CMastermind::ComputeStorageBufferUsage;
VkDeviceSize CMastermind::ComputeCustomUniformBufferSize;
VkDescriptorSetLayoutBinding CMastermind::ComputeCustomBinding;
int CMastermind::m_samplerCount;
//bool CMastermind::bDepthTest;
//bool CMastermind::bMSAA;
bool CMastermind::b48bpt;
bool CMastermind::bPushConstant;
bool CMastermind::bBlend;
VertexStructureTypes CMastermind::VertexStructureType;


// class CMastermind { //compute helper
// public:
//    CMastermind(){

//    }
// };

#endif