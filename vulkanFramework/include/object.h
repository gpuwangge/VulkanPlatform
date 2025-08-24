#ifndef H_OBJECT
#define H_OBJECT

#include "graphicsDescriptor.h"
#include "entity.h"
#include "renderProcess.h"
#include "camera.hpp"

//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;

class CObject : public CEntity {
    int m_object_id = 0;
    std::vector<int> m_texture_ids;
    std::vector<int> m_text_ids;
    int m_model_id = 0;

    bool b_isText = false;
    bool bUseMVP_VP = false;

    //bool bUseTextureSampler;
    std::vector<VkDescriptorSet> descriptorSets_graphics_texture_image_sampler; //This descritpor is for texture sampler. one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    //These will be used when recording draw cmd
    CRenderer *p_renderer;
    CRenderProcess *p_renderProcess;
    std::vector<VkDescriptorSet> *p_descriptorSets_graphics_general;
    //VkPipelineLayout *p_graphicsPipelineLayout;
    CTextureManager *p_textureManager;
    CTextManager *p_textManager;

    void CreateDescriptorSets_TextureImageSampler(
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        std::vector<VkSampler> &samplers, 
        std::vector<VkImageView> *swapchainImageViews = NULL);

    void CleanUp();

public:
    CObject();

    int m_default_graphics_pipeline_id = 0;
    //int m_graphics_pipeline_id1 = 0;
    //int m_graphics_pipeline_id2 = 0;

    bool bSticker = false;
    bool bSkybox = false;

    int GetObjectID(){return m_object_id;}
    std::vector<int> GetTextureID(){return m_texture_ids;}
    int GetModelID(){return m_model_id;}

    bool bUpdate = true;
    void Update(float deltaTime, int currentFrame, Camera &mainCamera);

    bool bRegistered = false;
    void Register(CApplication *p_app, int object_id, std::vector<int> texture_ids, std::vector<int> text_ids, int model_id, int default_graphics_pipeline_id);

    bool bVisible = true;

    //draw with renderer's buffer, or no buffer
    void Draw(int graphicsPipelineId = -1, uint32_t n = 0); //-1 means use default graphics pipeline id
    //draw with external buffers
    void Draw(std::vector<CWxjBuffer> &buffer, int graphicsPipelineId = -1, uint32_t n = 0); //const VkBuffer *pBuffers
};

#endif