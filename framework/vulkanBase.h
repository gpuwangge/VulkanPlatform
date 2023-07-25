#ifndef H_VULKANBASE
#define H_VULKANBASE

#include "application.h"

class CVulkanBase : public CApplication{
public:
    CVulkanBase();
    ~CVulkanBase();

    /**************
    Interface function to vulkan samples
    ************/
    void wxjCreateVertexBuffer();
    void wxjCreateIndexBuffer();
    void wxjCreateUniformBuffers();

    void wxjCreateCommandBuffer();

    void wxjCreateSwapChain();

    void wxjCreateVertexShader(std::string shaderName);
    void wxjCreateFragmentShader(std::string shaderName);

    //RenderPass: Attachments
    VkAttachmentDescription colorAttachment{};
    bool bUseColorAttachment = false;
    VkAttachmentDescription depthAttachment{};
    bool bUseDepthAttachment = false;
    VkAttachmentDescription colorAttachmentResolve{};
    bool bUseColorAttachmentResolve = false;
    void wxjCreateColorAttachment();
    void wxjCreatDepthAttachment();
    void wxjCreatColorAttachmentResolve();
    //RenderPass: Subpass
    VkAttachmentReference colorAttachmentRef{};
    VkAttachmentReference depthAttachmentRef{};
    VkAttachmentReference colorAttachmentResolveRef{};
    VkSubpassDescription subpass{};
    void wxjCreateSubpass();
    //RenderPass:Dependency
    VkSubpassDependency dependency{};
    void wxjCreateDependency();
    //RenderPass
    void wxjCreateRenderPass();

    void wxjCreateFramebuffers();

    void wxjCreateDescriptorPool(std::vector<VkDescriptorType> &descriptorTypes);
    void wxjCreateDescriptorSetLayout(std::vector<VkDescriptorType> &descriptorTypes, std::vector<VkShaderStageFlagBits>& shaderStageFlagBits);
    void wxjCreateDescriptorSets(std::vector<VkDescriptorType> &descriptorTypes);
    void wxjCreateGraphicsPipeline(VkPrimitiveTopology topology);

    void wxjCreateSyncObjects();

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, MyImageBuffer &imageBuffer);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void wxjCreateImage_texture(const std::string texturePath, MyImageBuffer &textureImageBuffer, int32_t &texWidth, int32_t &texHeight);
    void wxjCreateSampler_texture();
    void wxjCreateImageView(VkImage image);


    void wxjBeginCommandBuffer();
    void wxjBeginRenderPass(std::vector<VkClearValue> &clearValues);
    void wxjBindPipeline();
    void wxjSetViewport();
    void wxjSetScissor();
    void wxjBindVertexBuffer();
    void wxjBindIndexBuffer();
    void wxjBindDescriptorSets();
    void wxjDrawIndexed();
    void wxjEndRenderPass();
    void wxjEndCOmmandBuffer();
    /**************
    Application operation functions
    ************/
    void update();
    void recordCommandBuffer();
};

#endif