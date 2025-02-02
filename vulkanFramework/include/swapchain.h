#ifndef H_SWAPCHAIN
#define H_SWAPCHAIN

#include "vulkan\vulkan.h"
#include "physicalDevice.h"
#include "context.h"
#include "imageBuffer.h"
//#include "application.h"
#include "logManager.h"

class CSwapchain final{
    VkSwapchainKHR handle{VK_NULL_HANDLE};
public:
    CSwapchain();
    ~CSwapchain();

    void CleanUp();

    VkSwapchainKHR getHandle() const{ return handle;}
    //SwapchainInfo info;
    //CPhysicalDevice *m_physical_device;
    //void GetPhysicalDevice(CPhysicalDevice *physical_device);

    //VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
    void createImages(VkSurfaceKHR surface, int width, int height);
    void createImageViews(VkImageAspectFlags aspectFlags);

    void createMSAAImages(VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void createMSAAImageViews(VkImageAspectFlags aspectFlags);
    void createDepthImages(VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void createDepthImageViews(VkFormat format, VkImageAspectFlags aspectFlags);


	//CSwapchainImageManager imageManager;
    void GetMaxUsableSampleCount();

    std::vector<VkImage> images;
    std::vector<VkImageView> views;
    uint32_t imageSize;

    bool bComputeSwapChainImage = false; //added VK_IMAGE_USAGE_STORAGE_BIT for image storage

    //Attachments
    // bool bAttachmentDepthLight = false;
    // bool bAttachmentDepthCamera = false;
    // bool bAttachmentColorResovle = false;
    // bool bAttachmentColorPresent = true;
    int iAttachmentDepthLight = -1;
    int iAttachmentDepthCamera = -1;
    int iAttachmentColorResovle = -1;
    int iAttachmentColorPresent = -1;


    //Resource for light depth(shadowmap)
    //bool bEnableLightDepth;
    CWxjImageBuffer lightDepthImageBuffer;
    void EnableLightDepth();

    //Resource for Depth Test
    //bool bEnableDepthTest = false;
    VkFormat depthFormat;
	CWxjImageBuffer depthImageBuffer;
	//VkImageView depthImageView;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) ;
    VkFormat findDepthFormat();
    void EnableDepthTest();

    //Resource for MSAA
    //bool bEnableMSAA;
    VkSampleCountFlagBits msaaSamples;
    CWxjImageBuffer msaaColorImageBuffer;
	//VkImageView msaaColorImageView;
    void EnableMSAA();

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