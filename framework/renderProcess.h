#ifndef H_RENDERPROCESS
#define H_RENDERPROCESS

#include "common.h"
#include "context.h"

class CRenderProcess final{
public:
    CRenderProcess();
    ~CRenderProcess();

    bool bUseColorAttachment;
    bool bUseDepthAttachment;
    bool bUseColorAttachmentResolve;

    VkAttachmentDescription colorAttachment{};
    VkAttachmentDescription depthAttachment{};
    VkAttachmentDescription colorAttachmentResolve{};

    VkAttachmentReference colorAttachmentRef{};
    VkAttachmentReference depthAttachmentRef{};
    VkAttachmentReference colorAttachmentResolveRef{};

    VkSubpassDescription subpass{};
    VkSubpassDependency dependency{};
    VkRenderPass renderPass; 

    void createSubpass();
    void createDependency(
        VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
        VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    void createRenderPass();

    void addColorAttachment(VkSampleCountFlagBits msaaSamples, VkFormat swapChainImageFormat, 
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    void addDepthAttachment();
    void addColorAttachmentResolve();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();

    VkSampleCountFlagBits m_msaaSamples;
    VkFormat m_swapChainImageFormat;

    CDebugger * debugger;
};


#endif