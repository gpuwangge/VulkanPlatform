#ifndef H_SWAPCHAIN
#define H_SWAPCHAIN

#include "vulkan\vulkan.h"
#include "physicalDevice.h"
#include "context.h"
#include "imageBuffer.h"
#include "logManager.h"

class CSwapchain final{
    VkSwapchainKHR handle{VK_NULL_HANDLE};
public:
    CSwapchain();
    ~CSwapchain();

    void CleanUp();

    VkSwapchainKHR getHandle() const{ return handle;}

    void GetMaxUsableSampleCount();

    std::vector<VkImage> images;
    std::vector<VkImageView> views;
    uint32_t imageSize;

    bool bComputeSwapChainImage = false; //added VK_IMAGE_USAGE_STORAGE_BIT for image storage

    //Attachments (Resources)
    int iAttachmentDepthLight = -1;
    int iAttachmentDepthCamera = -1;
    int iAttachmentColorResovle = -1;
    int iAttachmentColorPresent = -1;

    //1.Resource for swapchain image
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
    void createImages(VkSurfaceKHR surface, int width, int height);
    void createImageViews(VkImageAspectFlags aspectFlags);

    //2.Resource for light depth(shadowmap)
    CWxjImageBuffer lightDepthImageBuffer;
    void create_attachment_buffer_light_depth(bool bEnableSamplerCountOn);

    //3.Resource for Depth Test
    VkFormat depthFormat;
	CWxjImageBuffer depthImageBuffer;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) ;
    VkFormat findDepthFormat();
    void create_attachment_description_camera_depth();

    //4.Resource for MSAA Resolve Color
    VkSampleCountFlagBits msaaSamples;
    CWxjImageBuffer msaaColorImageBuffer;
    void create_attachment_description_color_resolve();

	VkFormat swapChainImageFormat;//08
	VkExtent2D swapChainExtent;//08
	
    CLogManager logManager;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    void CreateFramebuffers(VkRenderPass &renderPass);

    void displaySwapchainInfo(SwapChainSupportDetails swapChainSupport);
    //CDebugger * debugger;

    //bool CheckFormatSupport(VkPhysicalDevice gpu, VkFormat format, VkFormatFeatureFlags requestedSupport);
};

#endif