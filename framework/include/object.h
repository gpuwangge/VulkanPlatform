#ifndef H_OBJECT
#define H_OBJECT
// #include "common.h"
// #include "context.h"
// #include "dataBuffer.hpp"
// #include "descriptor.h"
#include "application.h"

class CObject {
    int m_object_id;
    int m_texture_id;
    int m_model_id;
public:
    int GetObjectID(){return m_object_id;}
    int GetTextureID(){return m_texture_id;}
    int GetModelID(){return m_model_id;}

    bool bUseMVP_VP;
    //TODO
    glm::mat4 m_rotate; //16*4=64 bytes
	glm::mat4 m_scale; 
	glm::mat4 m_translate; 
    glm::mat4 m_model;

    //bool bUseVertexBuffer;
    //std::vector<Vertex2D> vertices2D;
    //std::vector<Vertex3D> vertices3D;

    //bool bUseIndexBuffer;
	//std::vector<uint32_t> indices3D;
    
    bool bUseTextureSampler;
    std::vector<VkDescriptorSet> descriptorSets; //This descritpor is for texture sampler. one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    //These will be used when recording draw cmd
    CRenderer *p_renderer;
    std::vector<VkDescriptorSet> *p_graphicsDescriptorSets;
    VkPipelineLayout *p_graphicsPipelineLayout;

    CObject();
    void CleanUp();

    // void InitVertices2D(std::vector<Vertex2D> &input_verticices2D);
    // void InitVertices3D(std::vector<Vertex3D> &input_verticices3D);
    // void InitIndices3D(std::vector<uint32_t> &input_indices3D);

    /*
    void Register(CApplication *p_app, int object_id = 0, int texture_id = 0, int model_id = 0){
        m_object_id = object_id;
        m_texture_id = texture_id;
        m_model_id = model_id;
        
        //Prepare pointers for drawcall
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);
    }*/

    //when a 3d object is register, it could be with texture(texture_id>=0) or no texture(texture_id=-1); it could have one texture, or several textures(INT_MAX)
    void Register(CApplication *p_app, int texture_id = 0, int model_id = 0, int object_id = 0){ //-1 means no texture or model 
        m_object_id = object_id;
        m_texture_id = texture_id;
        m_model_id = model_id;
        bUseMVP_VP = CGraphicsDescriptorManager::CheckMVP();
        
        //Prepare pointers for drawcall
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        //Prepare vertex/index buffers
        // vertices3D = input_verticices3D;
        // indices3D = input_indices3D;
        // p_renderer->CreateVertexBuffer<Vertex3D>(vertices3D); 
        // p_renderer->CreateIndexBuffer(indices3D);

        //Create texture descriptor set
        if(texture_id != -1){
            bUseTextureSampler = true; //vertex3D has texture coord, assume texture is used
            if(texture_id != INT_MAX){
                CreateTextureDescriptorSets(
                    p_app->textureManager.textureImages[m_texture_id], 
                    CGraphicsDescriptorManager::descriptorPool,
                    CGraphicsDescriptorManager::textureDescriptorSetLayout,
                    CGraphicsDescriptorManager::textureSamplers[0]
                );
            }else{
                CreateTextureDescriptorSets(
                    p_app->textureManager.textureImages, 
                    CGraphicsDescriptorManager::descriptorPool,
                    CGraphicsDescriptorManager::textureDescriptorSetLayout,
                    CGraphicsDescriptorManager::textureSamplers
                );
            }
        }

    }

    /*
    void Register(CApplication *p_app, int object_id = 0, int texture_id = 0, int model_id = 0){
        m_object_id = object_id;
        m_texture_id = texture_id;
        m_model_id = model_id;

        //Prepare pointers for drawcall
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        //Prepare vertex/index buffers
        //p_app->modelManager.LoadObjModel(modelName, vertices3D, indices3D);
        //p_renderer->CreateVertexBuffer<Vertex3D>(vertices3D); 
        //p_renderer->CreateIndexBuffer(indices3D);

        //Create texture descriptor set
        bUseTextureSampler = true; //assume model has texture coordinate information
        CreateTextureDescriptorSets(
			p_app->textureManager.textureImages[m_texture_id], 
			CGraphicsDescriptorManager::descriptorPool,
			CGraphicsDescriptorManager::textureDescriptorSetLayout,
			CGraphicsDescriptorManager::textureSamplers[0],
			CGraphicsDescriptorManager::CheckMVP()
			);
    }*/

   /*
    void Register(CApplication *p_app, std::vector<Vertex2D> &input_verticices2D, int texture_id = 0, int model_id = 0, int object_id = 0){
        m_object_id = object_id;
        m_texture_id = texture_id;
        m_model_id = model_id;
        bUseMVP_VP = CGraphicsDescriptorManager::CheckMVP();
        
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        //vertices2D = input_verticices2D; //TODO: Vertex2D has no texture coordinate yet 
        //p_renderer->CreateVertexBuffer<Vertex2D>(vertices2D); 

        //Create texture descriptor set
        if(texture_id != -1){
            bUseTextureSampler = true; //vertex3D has texture coord, assume texture is used
            if(texture_id != INT_MAX){
                CreateTextureDescriptorSets(
                    p_app->textureManager.textureImages[m_texture_id], 
                    CGraphicsDescriptorManager::descriptorPool,
                    CGraphicsDescriptorManager::textureDescriptorSetLayout,
                    CGraphicsDescriptorManager::textureSamplers[0]
                );
            }else{
                CreateTextureDescriptorSets(
                    p_app->textureManager.textureImages, 
                    CGraphicsDescriptorManager::descriptorPool,
                    CGraphicsDescriptorManager::textureDescriptorSetLayout,
                    CGraphicsDescriptorManager::textureSamplers
                );
            }
        }
    }*/

    void CreateTextureDescriptorSets(
        CTextureImage &textureImage, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        VkSampler &sampler, 
        std::vector<VkImageView> *swapchainImageViews = NULL);

    void CreateTextureDescriptorSets(
        std::vector<CTextureImage> &textureImages, 
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        std::vector<VkSampler> &samplers, 
        std::vector<VkImageView> *swapchainImageViews = NULL);
    

    void SetRotation(glm::mat4 m){
        CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = m;
    }


    //draw with renderer's buffer, or no buffer
    void Draw(uint32_t n = 0){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		if(p_graphicsDescriptorSets->size() > 0) dsSets.push_back(*p_graphicsDescriptorSets); //set = 0, general uniform
        if(bUseTextureSampler) dsSets.push_back(descriptorSets); //set = 1, texture sampler uniform

        if(dsSets.size() > 0){
            int dynamicOffsetIndex = -1; //-1 means not use dynamic offset (no MVP/VP used)
            if(bUseMVP_VP) dynamicOffsetIndex = m_object_id; //assume descriptor uniform(MVP/VP) offset is m_id
		    p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, dynamicOffsetIndex);
        }//else std::cout<<"No Descritpor is used."<<std::endl;
    
        //if(!vertices3D.empty() || !vertices2D.empty()){
        if(m_model_id!=-1) p_renderer->BindVertexBuffer(m_model_id);
        //}//else std::cout<<"No vertex buffer is used."<<std::endl;
        
        //if(indices3D.empty()){
        if(p_renderer->indices3Ds.empty()){
            //std::cout<<"No index buffer is used."<<std::endl;
            p_renderer->Draw(n);
        }else{
		    p_renderer->BindIndexBuffer(m_model_id);
            p_renderer->DrawIndexed(m_model_id);
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