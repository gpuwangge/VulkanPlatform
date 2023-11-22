#ifndef H_DESCRIPTOR
#define H_DESCRIPTOR

#include "context.h"
#include "dataBuffer.hpp"
#include "camera.hpp"
#include "../include/texture.h"

typedef enum UniformBufferBits {
    UNIFORM_BUFFER_CUSTOM_BIT = 0x00000001,
    UNIFORM_BUFFER_MVP_BIT = 0x00000002,
    UNIFORM_BUFFER_VP_BIT = 0x00000004,
    UNIFORM_BUFFER_SAMPLER_BIT = 0x00000008,
    UNIFORM_BUFFER_STORAGE_BIT = 0x00000010,
    //UNIFORM_BUFFER_STORAGE_2_BIT = ,
    UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT = 0x00000020,
    UNIFORM_IMAGE_STORAGE_TEXTURE_BIT = 0x00000040,
    UNIFORM_BUFFERG_BITS_MAX_ENUM = 0x7FFFFFFF
} UniformBufferBits;

class CDescriptor final{

public:
    CDescriptor();

    ~CDescriptor();

    std::vector<VkDescriptorPoolSize> poolSizes;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    int uniformBufferUsageFlags;

    //bool bUseCustomUniformBuffer;
    std::vector<CWxjBuffer> customUniformBuffers; 
	std::vector<void*> customUniformBuffersMapped;
    VkDeviceSize m_customUniformBufferSize;
    void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    template <typename T>
    void updateCustomUniformBuffer(uint32_t currentFrame, float durationTime, T customUniformBufferObject){
        if(uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_BIT)
            memcpy(customUniformBuffersMapped[currentFrame], &customUniformBufferObject, sizeof(customUniformBufferObject));
    }

    //bool bUseMVP;
    std::vector<CWxjBuffer> mvpUniformBuffers; 
	std::vector<void*> mvpUniformBuffersMapped;
    void addMVPUniformBuffer();
    MVPUniformBufferObject mvpUBO{};
    void updateMVPUniformBuffer(uint32_t currentFrame, float durationTime, Camera &mainCamera);

    //bool bUseVP;
    std::vector<CWxjBuffer> vpUniformBuffers; 
	std::vector<void*> vpUniformBuffersMapped;
    void addVPUniformBuffer();
    VPUniformBufferObject vpUBO{};
    void updateVPUniformBuffer(uint32_t currentFrame, float durationTime, Camera &mainCamera);

    //bool bUseSampler;
    int textureSamplerCount;
    std::vector<VkSampler> textureSamplers;
    void addImageSamplerUniformBuffer(uint32_t mipLevels);

    //bool bUseStorageBuffer;
    //define two set of storage buffers, one for input, the other for output
	std::vector<CWxjBuffer> storageBuffers; 
    std::vector<void*> storageBuffersMapped;
    VkDeviceSize m_storageBufferSize;
    //std::vector<CWxjBuffer> storageBuffers_2; 
    //std::vector<void*> storageBuffersMapped_2;
    //VkDeviceSize m_storageBufferSize_2;
    void addStorageBuffer(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT); //the same function to add storage 1&2
    template <typename T>
    void updateStorageBuffer(uint32_t currentFrame, float durationTime, T storageBufferObject){ 
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_1_BIT)
        memcpy(storageBuffersMapped[currentFrame], &storageBufferObject, sizeof(storageBufferObject));
    }
    //template <typename T>
    //void updateStorageBuffer_2(uint32_t currentFrame, float durationTime, T storageBufferObject){ 
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT)
    //    memcpy(storageBuffersMapped_2[currentFrame], &storageBufferObject, sizeof(storageBufferObject));
    //}

    //for storage image
    //std::vector<CWxjBuffer> storageImages; 
    //std::vector<void*> storageImagesMapped;
    //VkDeviceSize m_storageImageSize;
    void addStorageImage(VkBufferUsageFlags usage);
    // void updateStorageImage();



    int getDescriptorSize();

    void createDescriptorPool();//VkDescriptorType    type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);
        //VkDescriptorType      descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        //VkShaderStageFlags    stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        //uint32_t              descriptorCount = 1,
        //const VkSampler*      pImmutableSamplers = nullptr);
    void createDescriptorSets(std::vector<CTextureImage> *textureImages = NULL, std::vector<VkImageView> *swapchainImageViews = NULL);

    void DestroyAndFree();

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;

    //CDebugger * debugger;
};



#endif