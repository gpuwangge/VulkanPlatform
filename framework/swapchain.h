#ifndef H_SWAPCHAIN
#define H_SWAPCHAIN

#include "vulkan\vulkan.h"
#include "physicalDevice.h"
//#include "application.h"

class CSwapchain final{
    //VkSwapchainKHR swapChain;//08
    VkSwapchainKHR handle{VK_NULL_HANDLE};
public:
    CSwapchain();
    ~CSwapchain();

    //SwapchainInfo info;
    CPhysicalDevice *m_physical_device;
    void GetPhysicalDevice(CPhysicalDevice *physical_device);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
    void queryInfo(VkSurfaceKHR surface, int width, int height);

    VkSwapchainKHR getHandle() const{ return handle;}

	std::vector<VkImage> swapChainImages;//08
	VkFormat swapChainImageFormat;//08
	VkExtent2D swapChainExtent;//08
	std::vector<VkImageView> swapChainImageViews;//08

    void displaySwapchainInfo(SwapChainSupportDetails swapChainSupport);
    CDebugger * debugger;
};

#endif