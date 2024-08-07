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

    //TODO
    bool bUseMVP_VP;
    glm::mat4 m_rotate; //16*4=64 bytes
	glm::mat4 m_scale; 
	glm::mat4 m_translate; 
    glm::mat4 m_model;


    std::vector<Vertex2D> vertices2D;
    std::vector<Vertex3D> vertices3D;
	std::vector<uint32_t> indices3D;
    
    std::vector<VkDescriptorSet> descriptorSets; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    //These will be used when recording draw cmd
    CRenderer *p_renderer;
    std::vector<VkDescriptorSet> *p_graphicsDescriptorSets;
    VkPipelineLayout *p_graphicsPipelineLayout;

    CObject();
    void CleanUp();

    void InitVertices2D(std::vector<Vertex2D> &input_verticices2D);
    void InitVertices3D(std::vector<Vertex3D> &input_verticices3D);
    void InitIndices3D(std::vector<uint32_t> &input_indices3D);



    void Init(CApplication *p_app, std::vector<Vertex3D> &input_verticices3D, std::vector<uint32_t> &input_indices3D, int id = 0){
        m_id = id;
        
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        vertices3D = input_verticices3D;
        indices3D = input_indices3D;

        p_renderer->CreateVertexBuffer<Vertex3D>(vertices3D); 
        p_renderer->CreateIndexBuffer(indices3D);
    }

    void Init(CApplication *p_app, std::string modelName, int id = 0){
        m_id = id;

        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        p_app->modelManager.LoadObjModel(modelName, vertices3D, indices3D);

        p_renderer->CreateVertexBuffer<Vertex3D>(vertices3D); 
        p_renderer->CreateIndexBuffer(indices3D);
    }

    void Init(CApplication *p_app, std::vector<Vertex2D> &input_verticices2D, int id = 0){
        m_id = id;
        
        p_renderer = &(p_app->renderer);
        p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayout);
		p_graphicsDescriptorSets = &(p_app->graphicsDescriptorManager.descriptorSets);

        vertices2D = input_verticices2D;
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
    
    void RecordDrawIndexCmd(){
		std::vector<std::vector<VkDescriptorSet>> dsSets; 
		dsSets.push_back(*p_graphicsDescriptorSets); //set = 0
		dsSets.push_back(descriptorSets); //set = 1

        
        int dynamicOffsetIndex = -1; //-1 means not use dynamic offset (no MVP/VP used)
        if(bUseMVP_VP) dynamicOffsetIndex = m_id; //assume descriptor uniform(MVP/VP) offset is m_id
		p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, dynamicOffsetIndex);
		p_renderer->BindVertexBuffer(m_id);
		p_renderer->BindIndexBuffer(m_id);
		p_renderer->DrawIndexed(indices3D);
    }
};

#endif