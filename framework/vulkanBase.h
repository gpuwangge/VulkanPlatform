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
    void wxjLoadObjModel(const std::string modelName);

    void wxjCreateVertexBuffer();
    void wxjCreateIndexBuffer();
    void wxjCreateUniformBuffers();

    void wxjCreateCommandBuffer();

    void wxjCreateSwapChainImagesAndImageViews();

    void wxjCreateVertexShader(std::string shaderName);
    void wxjCreateFragmentShader(std::string shaderName);

    //RenderPass: Attachments
    VkAttachmentDescription colorAttachment{};
    bool bUseColorAttachment = false;
    VkAttachmentDescription depthAttachment{};
    bool bUseDepthAttachment = false;
    VkAttachmentDescription colorAttachmentResolve{};
    bool bUseColorAttachmentResolve = false;
    void wxjCreateColorAttachment(VkImageLayout imageLayout);
    void wxjCreateDepthAttachment();
    void wxjCreateColorAttachmentResolve();
    //RenderPass: Subpass
    VkAttachmentReference colorAttachmentRef{};
    VkAttachmentReference depthAttachmentRef{};
    VkAttachmentReference colorAttachmentResolveRef{};
    VkSubpassDescription subpass{};
    void wxjCreateSubpass();
    //RenderPass:Dependency
    VkSubpassDependency dependency{};
    void wxjCreateDependency(VkPipelineStageFlags srcPipelineStageFlag, VkPipelineStageFlags dstPipelineStageFlag);
    //RenderPass
    void wxjCreateRenderPass();

    void wxjCreateFramebuffers();

    void wxjCreateDescriptorPool(std::vector<VkDescriptorType> &descriptorTypes);
    void wxjCreateDescriptorSetLayout(std::vector<VkDescriptorType> &descriptorTypes, std::vector<VkShaderStageFlagBits>& shaderStageFlagBits);
    void wxjCreateDescriptorSets(std::vector<VkDescriptorType> &descriptorTypes);
    void wxjCreateGraphicsPipeline(VkPrimitiveTopology topology);

    //void wxjCreateSyncObjects();

    void wxjCreateImage_texture(const std::string texturePath, OUT MyImageBuffer &textureImageBuffer, OUT int32_t &texWidth, OUT int32_t &texHeight);
    void wxjCreateSampler_texture();
    void wxjCreateImageView(IN VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, OUT VkImageView &imageView);
    void wxjCreateImage(VkSampleCountFlagBits numSamples, VkFormat format, VkImageUsageFlags usage, OUT MyImageBuffer &imageBuffer);

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

    void wxjGetMaxUsableSampleCount();
    /**************
    Application operation functions
    ************/
    // void initialize();
    // void update();
    // void recordCommandBuffer();
};

#endif