#ifndef H_SWAPCHAIN
#define H_SWAPCHAIN

#include "vulkan\vulkan.h"
#include "physicalDevice.h"
#include "context.h"
#include "imageManager.h"
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

	CSwapchainImageManager imageManager;
	VkFormat swapChainImageFormat;//08
	VkExtent2D swapChainExtent;//08
	
    std::vector<VkFramebuffer> swapChainFramebuffers;
    void CreateFramebuffers(bool bUseDepthAttachment, bool bUseColorResolveAttachment, VkRenderPass &renderPass);

    void displaySwapchainInfo(SwapChainSupportDetails swapChainSupport);
    CDebugger * debugger;
};

#endif