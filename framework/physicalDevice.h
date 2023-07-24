#ifndef H_PHYSICAL_DEVICE
#define H_PHYSICAL_DEVICE

//#include "instance.h"
#include "logicalDevice.h"

#include "common.h"

class CInstance;//forward declaration

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> graphicsAndComputeFamily;
	std::optional<uint32_t> presentFamily;
	

	bool isComplete() {
		return graphicsAndComputeFamily.has_value() && graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class CPhysicalDevice{
public:
    //CInstance *m_instance;
    CPhysicalDevice(VkPhysicalDevice physical_device);
    //CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device);

    ~CPhysicalDevice();

    VkPhysicalDevice handle{VK_NULL_HANDLE};
    VkPhysicalDevice getHandle() const{ return handle;}

    CDebugger * debugger;

    void setInstance(CInstance *instance);


    QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface);
    bool checkDeviceExtensionSupport(const std::vector<const char*>  requireDeviceExtensions);
    SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface);



    std::vector<std::unique_ptr<CLogicalDevice>> logicalDevices;

    void createLogicalDevices();

    /*Below are MSAA related functions TODO
    VkSampleCountFlagBits getMaxUsableSampleCount_msaa() {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }*/

    

};

#endif
