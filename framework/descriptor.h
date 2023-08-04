#ifndef H_DESCRIPTOR
#define H_DESCRIPTOR

#include "context.h"
#include "dataStructure.hpp"

class CDescriptor{

public:
    CDescriptor();

    ~CDescriptor();

    bool bUseMVP;
    std::vector<CWxjBuffer> uniformBuffers; 
	std::vector<void*> uniformBuffersMapped;
    void addMVPUniformBuffer();

    bool bUseSampler;
    VkSampler textureSampler;
    void addImageSamplerUniformBuffer(uint32_t mipLevels);


    void createDescriptorPool();
    void createDescriptorSetLayout();
    void createDescriptorSets(VkImageView &textureImageView);

    void DestroyAndFree();

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkDescriptorType> descriptorTypes;
    std::vector<VkShaderStageFlagBits> shaderStageFlagBits;

    

    CDebugger * debugger;
};



#endif