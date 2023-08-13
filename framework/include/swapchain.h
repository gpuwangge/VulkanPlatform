#ifndef H_SWAPCHAIN
#define H_SWAPCHAIN

#include "vulkan\vulkan.h"
#include "physicalDevice.h"
#include "context.h"
#include "imageBuffer.h"
//#include "application.h"

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

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
    void createSwapchainImages(VkSurfaceKHR surface, int width, int height);
    void createImageViews(VkImageAspectFlags aspectFlags);

    void createMSAAImages(VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void createMSAAImageViews(VkImageAspectFlags aspectFlags);
    void createDepthImages(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void createDepthImageViews(VkFormat format, VkImageAspectFlags aspectFlags);


	//CSwapchainImageManager imageManager;
    void GetMaxUsableSampleCount();

    std::vector<VkImage> images;//08
    std::vector<VkImageView> views;//08
    uint32_t imageSize;

    //Resource for Depth Test
    bool bEnableDepthTest = false;
	CWxjImageBuffer depthImageBuffer;
	//VkImageView depthImageView;

    //Resource for MSAA
    bool bEnableMSAA;
    VkSampleCountFlagBits msaaSamples;
    CWxjImageBuffer msaaColorImageBuffer;
	//VkImageView msaaColorImageView;

	VkFormat swapChainImageFormat;//08
	VkExtent2D swapChainExtent;//08
	


    std::vector<VkFramebuffer> swapChainFramebuffers;
    void CreateFramebuffers(VkRenderPass &renderPass);

    void displaySwapchainInfo(SwapChainSupportDetails swapChainSupport);
    //CDebugger * debugger;
};

#endif