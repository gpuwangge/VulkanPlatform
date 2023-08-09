#ifndef H_DESCRIPTOR
#define H_DESCRIPTOR

#include "context.h"
#include "dataBuffer.hpp"
#include "camera.hpp"

class CDescriptor final{

public:
    CDescriptor();

    ~CDescriptor();

    std::vector<VkDescriptorPoolSize> poolSizes;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    bool bUseCustomUniformBuffer;
    std::vector<CWxjBuffer> customUniformBuffers; 
	std::vector<void*> customUniformBuffersMapped;
    VkDeviceSize m_customUniformBufferSize;
    void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    template <typename T>
    void updateCustomUniformBuffer(uint32_t currentFrame, float durationTime, T customUBO){
        if(bUseCustomUniformBuffer)
            memcpy(customUniformBuffersMapped[currentFrame], &customUBO, sizeof(customUBO));
    }

    bool bUseMVP;
    std::vector<CWxjBuffer> mvpUniformBuffers; 
	std::vector<void*> mvpUniformBuffersMapped;
    void addMVPUniformBuffer();
    MVPUniformBufferObject mvpUBO{};
    void updateMVPUniformBuffer(uint32_t currentFrame, float durationTime, Camera &mainCamera);

    bool bUseSampler;
    VkSampler textureSampler;
    void addImageSamplerUniformBuffer(uint32_t mipLevels);

    int getDescriptorSize();

    void createDescriptorPool(VkDescriptorType    type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);//
        //VkDescriptorType      descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        //VkShaderStageFlags    stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        //uint32_t              descriptorCount = 1,
        //const VkSampler*      pImmutableSamplers = nullptr);
    void createDescriptorSets(VkImageView *textureImageView = nullptr);

    void DestroyAndFree();

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;

    CDebugger * debugger;
};



#endif