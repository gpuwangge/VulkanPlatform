#ifndef H_OBJECT
#define H_OBJECT
// #include "common.h"
// #include "context.h"
// #include "dataBuffer.hpp"
// #include "descriptor.h"
#include "application.h"

class CObject {
    int m_id;
public:
    int GetID(){return m_id;}

    bool bUseMVP_VP;
    //TODO
    glm::mat4 m_rotate; //16*4=64 bytes
	glm::mat4 m_scale; 
	glm::mat4 m_translate; 
    glm::mat4 m_model;

    //bool bUseVertexBuffer;
    std::vector<Vertex2D> vertices2D;
    std::vector<Vertex3D> vertices3D;

    //bool bUseIndexBuffer;
	std::vector<uint32_t> indices3D;
    
    bool bUseTextureSampler;
    std::vector<VkDescriptorSet> descriptorSets; //This descritpor is for texture sampler. one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    //These will be used when recording draw cmd
    CRenderer *p_renderer;
    std::vector<VkDescriptorSet> *p_graphicsDescriptorSets;
    VkPipelineLayout *p_graphicsPipelineLayout;

    CObject();
    void CleanUp();

    void InitVertices2D(std::vector<Vertex2D> &input_verticices2D);
    void InitVertices3D(std::vector<Vertex3D> &input_verticices3D);
    void InitIndices3D(std::vector<uint32_t> &input_indices3D);

    void Register(CApplication *p_app, int id = 0){
        m_id = id;

         //Prepare pointers for drawcall
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);
    }

    void Register(CApplication *p_app, std::vector<Vertex3D> &input_verticices3D, std::vector<uint32_t> &input_indices3D, int id = 0, bool bMultiSamplers = false){
        m_id = id;
        
        //Prepare pointers for drawcall
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        //Prepare vertex/index buffers
        vertices3D = input_verticices3D;
        indices3D = input_indices3D;
        p_renderer->CreateVertexBuffer<Vertex3D>(vertices3D); 
        p_renderer->CreateIndexBuffer(indices3D);

        //Create texture descriptor set
        bUseTextureSampler = true; //vertex3D has texture coord, assume texture is used
        if(!bMultiSamplers){
            CreateTextureDescriptorSets(
			    p_app->textureManager.textureImages[GetID()], 
                CGraphicsDescriptorManager::descriptorPool,
                CGraphicsDescriptorManager::textureDescriptorSetLayout,
                CGraphicsDescriptorManager::textureSamplers[0],
                CGraphicsDescriptorManager::CheckMVP()
			);
        }else{
            CreateTextureDescriptorSets(
                p_app->textureManager.textureImages, 
                CGraphicsDescriptorManager::descriptorPool,
                CGraphicsDescriptorManager::textureDescriptorSetLayout,
                CGraphicsDescriptorManager::textureSamplers,
                CGraphicsDescriptorManager::CheckMVP()
			);
        }

    }

    void Register(CApplication *p_app, std::string modelName, int id = 0){
        m_id = id;

        //Prepare pointers for drawcall
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        //Prepare vertex/index buffers
        p_app->modelManager.LoadObjModel(modelName, vertices3D, indices3D);
        p_renderer->CreateVertexBuffer<Vertex3D>(vertices3D); 
        p_renderer->CreateIndexBuffer(indices3D);

        //Create texture descriptor set
        bUseTextureSampler = true; //assume model has texture coordinate information
        CreateTextureDescriptorSets(
			p_app->textureManager.textureImages[GetID()], 
			CGraphicsDescriptorManager::descriptorPool,
			CGraphicsDescriptorManager::textureDescriptorSetLayout,
			CGraphicsDescriptorManager::textureSamplers[0],
			CGraphicsDescriptorManager::CheckMVP()
			);
    }

    void Register(CApplication *p_app, std::vector<Vertex2D> &input_verticices2D, int id = 0){
        m_id = id;
        
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        vertices2D = input_verticices2D; //TODO: Vertex2D has no texture coordinate yet 
        //indices3D = input_indices3D;

        p_renderer->CreateVertexBuffer<Vertex2D>(vertices2D); 
        //p_renderer->CreateIndexBuffer(indices3D);
    }

    void CreateTextureDescriptorSets(
        CTextureImage &textureImage, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        VkSampler &sampler, 
        bool bCheckMVP,
        std::vector<VkImageView> *swapchainImageViews = NULL);

    void CreateTextureDescriptorSets(
        std::vector<CTextureImage> &textureImages, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        std::vector<VkSampler> &samplers, 
        bool bCheckMVP,
        std::vector<VkImageView> *swapchainImageViews = NULL);
    
    //draw with renderer's buffer, or no buffer
    void Draw(uint32_t n = 0){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		if(p_graphicsDescriptorSets->size() > 0) dsSets.push_back(*p_graphicsDescriptorSets); //set = 0, general uniform
        if(bUseTextureSampler) dsSets.push_back(descriptorSets); //set = 1, texture sampler uniform

        if(dsSets.size() > 0){
            int dynamicOffsetIndex = -1; //-1 means not use dynamic offset (no MVP/VP used)
            if(bUseMVP_VP) dynamicOffsetIndex = m_id; //assume descriptor uniform(MVP/VP) offset is m_id
		    p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, dynamicOffsetIndex);
        }//else std::cout<<"No Descritpor is used."<<std::endl;
    
        if(!vertices3D.empty() || !vertices2D.empty()){
		    p_renderer->BindVertexBuffer(m_id);
        }//else std::cout<<"No vertex buffer is used."<<std::endl;
        if(indices3D.empty()){
            //std::cout<<"No index buffer is used."<<std::endl;
            p_renderer->Draw(n);
        }else{
		    p_renderer->BindIndexBuffer(m_id);
		    p_renderer->DrawIndexed(indices3D);
        }
    }

    //draw with external buffers
    void Draw(std::vector<CWxjBuffer> &buffer, uint32_t n = 0){ //const VkBuffer *pBuffers
        //TODO: bind descriptor set

        //VkDeviceSize offsets[] = { 0 };
		//vkCmdBindVertexBuffers(p_renderer->commandBuffers[p_renderer->graphicsCmdId][p_renderer->currentFrame], 0, 1, &buffer[p_renderer->currentFrame].buffer, offsets);
        p_renderer->BindExternalBuffer(buffer);
		p_renderer->Draw(n);
    }
};

#endif