#ifndef H_GRAPHICSDESCRIPTOR
#define H_GRAPHICSDESCRIPTOR

#include "context.h"
#include "dataBuffer.hpp"
#include "../include/texture.h"

class CGraphicsDescriptorManager{
public:
    CGraphicsDescriptorManager(){};
    ~CGraphicsDescriptorManager(){};

    /************
     * Pool
     ************/
    static int graphicsUniformTypes;
    static VkDescriptorPool graphicsDescriptorPool;
    static std::vector<VkDescriptorPoolSize> graphicsDescriptorPoolSizes;
    static void createDescriptorPool(unsigned int object_count = 0);
    
    /************
     * Layout
     ************/
    static std::vector<VkDescriptorSetLayoutBinding> graphicsBindings;
    static VkDescriptorSetLayout descriptorSetLayout_general;
    static VkDescriptorSetLayout descriptorSetLayout_texture;
    static void createDescriptorSetLayout_General(VkDescriptorSetLayoutBinding *customBinding = nullptr);
    static void createDescriptorSetLayout_Texture();

    /************
     * Set
     ************/
    std::vector<VkDescriptorSet> descriptorSets_general; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)
    void createDescriptorSets_General();

    /************
     * 1 GRAPHCIS_UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    static void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    template <typename T>
    void updateCustomUniformBuffer(uint32_t currentFrame, float durationTime, T customUniformBufferObject){
        //std::cout<<"sizeof(customUniformBufferObject)="<<sizeof(customUniformBufferObject)<<std::endl;
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM)
            memcpy(customUniformBuffersMapped[currentFrame], &customUniformBufferObject, sizeof(customUniformBufferObject));
    }

    /************
     * 2 GRAPHCIS_UNIFORMBUFFER_LIGHTING
     ************/
    static LightingUniformBufferObject m_lightingUBO;
    static std::vector<CWxjBuffer> m_lightingUniformBuffers; 
	static std::vector<void*> m_lightingUniformBuffersMapped;
    static VkDeviceSize m_lightingUniformBufferSize;
    static void addLightingUniformBuffer();

    /************
     * 3 GRAPHCIS_UNIFORMBUFFER_MVP
     ************/
    static std::vector<CWxjBuffer> mvpUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
	static std::vector<void*> mvpUniformBuffersMapped;
    static void addMVPUniformBuffer();
    static MVPUniformBufferObject mvpUBO;

    /************
     * 4 GRAPHCIS_UNIFORMBUFFER_VP
     ************/
    static std::vector<CWxjBuffer> vpUniformBuffers; 
	static std::vector<void*> vpUniformBuffersMapped;
    static void addVPUniformBuffer();
    static VPUniformBufferObject vpUBO;
    static bool CheckMVP(); //to check if all objects associate this graphcis descriptor use MVP/VP or not. If return true, means it will use dynamic descriptor offset

    /************
     * 5 GRAPHCIS_COMBINEDIMAGE_SAMPLER
     ************/
    static unsigned int samplerSize;
    static std::vector<VkSampler> textureSamplers;
    static void addImageSamplerUniformBuffer(std::vector<int> mipLevels);

    
    /************
     * Helper Functions
     ************/
    static int getPoolSize();
    static int getLayoutSize_General();
    static int getSetSize_General();
    void DestroyAndFree();
};

#endif