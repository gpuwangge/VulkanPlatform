#ifndef H_OBJECT
#define H_OBJECT

//#include <glm/gtc/quaternion.hpp>
#include "graphicsDescriptor.h"
#include "entity.h"
#include "renderProcess.h"


//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;

class CObject : public CEntity {
    int m_object_id = 0;
    std::vector<int> m_texture_ids;
    int m_model_id = 0;
    int m_graphics_pipeline_id = 0;

    bool bUseMVP_VP = false;

    //bool bUseTextureSampler;
    std::vector<VkDescriptorSet> descriptorSets; //This descritpor is for texture sampler. one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    //These will be used when recording draw cmd
    CRenderer *p_renderer;
    CRenderProcess *p_renderProcess;
    std::vector<VkDescriptorSet> *p_graphicsDescriptorSets;
    VkPipelineLayout *p_graphicsPipelineLayout;
    CTextureManager *p_textureManager;

    // void CreateTextureDescriptorSets(
    //     CTextureImage &textureImage, 
    //     VkDescriptorPool &descriptorPool, 
    //     VkDescriptorSetLayout &descriptorSetLayout, 
    //     VkSampler &sampler, 
    //     std::vector<VkImageView> *swapchainImageViews = NULL);

    void CreateTextureDescriptorSets(
        //CTextureImage &textureImage0, CTextureImage &textureImage1, CTextureImage &textureImage2,
        VkDescriptorPool &descriptorPool, 
        VkDescriptorSetLayout &descriptorSetLayout, 
        std::vector<VkSampler> &samplers, 
        std::vector<VkImageView> *swapchainImageViews = NULL);

    void CleanUp();

public:
    CObject();

    bool bSticker = false;
    bool bSkybox = false;

    int GetObjectID(){return m_object_id;}
    std::vector<int> GetTextureID(){return m_texture_ids;}
    int GetModelID(){return m_model_id;}

    bool bUpdate = true;
    void Update(float deltaTime, int currentFrame, Camera &mainCamera);

    bool bRegistered = false;
    void Register(CApplication *p_app, int object_id, std::vector<int> texture_ids, int model_id, int graphics_pipeline_id); 

    //draw with renderer's buffer, or no buffer
    void Draw(uint32_t n = 0);
    //draw with external buffers
    void Draw(std::vector<CWxjBuffer> &buffer, uint32_t n = 0); //const VkBuffer *pBuffers
};

#endif