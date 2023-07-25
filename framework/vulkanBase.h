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


    /**************
    Application operation functions
    ************/
    void updateUniformBuffer(uint32_t currentFrame, float durationTime);
    void update();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void initVulkan();
};

#endif