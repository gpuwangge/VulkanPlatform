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
    //std::vector<CWxjBuffer> uniformBuffers_compute; //用来创建delta t的uniform
	//std::vector<void*> uniformBuffersMapped_compute; //用来更新delta t的uniform
	std::vector<CWxjBuffer> storageBuffers; //用来把particle initial数据(pos, color, velocity)upload到gpu
    std::vector<void*> storageBuffersMapped;
    VkDeviceSize m_storageBufferSize;
    void addStorageBuffer(VkDeviceSize storageBufferSize);
    template <typename T>
    void updateStorageBuffer(uint32_t currentFrame, float durationTime, T storageBufferObject){
        if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_BIT)
            memcpy(storageBuffersMapped[currentFrame], &storageBufferObject, sizeof(storageBufferObject));
    }



    int getDescriptorSize();

    void createDescriptorPool();//VkDescriptorType    type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);
        //VkDescriptorType      descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        //VkShaderStageFlags    stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        //uint32_t              descriptorCount = 1,
        //const VkSampler*      pImmutableSamplers = nullptr);
    void createDescriptorSets(std::vector<CTextureImage> &textureImages);

    void DestroyAndFree();

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;

    //CDebugger * debugger;
};



#endif