#ifndef H_OBJECT
#define H_OBJECT

//#include <glm/gtc/quaternion.hpp>
#include "descriptor.h"
#include "entity.h"
#include "renderProcess.h"


//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;

class CObject : public CEntity {
    int m_object_id;
    std::vector<int> m_texture_ids;
    int m_model_id;
    int m_graphics_pipeline_id;

    bool bUseMVP_VP;

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

    bool bSticker;
    bool bSkybox;

    int GetObjectID(){return m_object_id;}
    std::vector<int> GetTextureID(){return m_texture_ids;}
    int GetModelID(){return m_model_id;}

    bool bUpdate;
    void Update(float deltaTime);

    //when a 3d object is register, it could be with texture(texture_id>=0) or no texture(texture_id=-1); it could have one texture, or several textures(INT_MAX)
    void Register(CApplication *p_app, std::vector<int> texture_ids, int model_id, int object_id, int graphics_pipeline_id, glm::vec3 length, glm::vec3 lengthMin, glm::vec3 lengthMax); //-1 means no texture or model 

    //draw with renderer's buffer, or no buffer
    void Draw(uint32_t n = 0);
    //draw with external buffers
    void Draw(std::vector<CWxjBuffer> &buffer, uint32_t n = 0); //const VkBuffer *pBuffers
};

#endif