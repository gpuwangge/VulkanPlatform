#ifndef H_OBJECT
#define H_OBJECT
#include "descriptor.h"

//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include object.h instead
class CApplication;

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

    //when a 3d object is register, it could be with texture(texture_id>=0) or no texture(texture_id=-1); it could have one texture, or several textures(INT_MAX)
    void Register(CApplication *p_app, int texture_id = 0, int model_id = 0, int object_id = 0); //-1 means no texture or model 

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
    
    void SetRotation(glm::mat4 m);

    //draw with renderer's buffer, or no buffer
    void Draw(uint32_t n = 0);
    //draw with external buffers
    void Draw(std::vector<CWxjBuffer> &buffer, uint32_t n = 0); //const VkBuffer *pBuffers

};

#endif