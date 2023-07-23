#ifndef H_PHYSICAL_DEVICE
#define H_PHYSICAL_DEVICE

//#include "instance.h"
#include "logicalDevice.h"

#include "common.h"

class CInstance;//forward declaration

class CPhysicalDevice{
public:
    CInstance *m_instance;
    CPhysicalDevice(VkPhysicalDevice physical_device);
    //CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device);

    ~CPhysicalDevice();

    VkPhysicalDevice handle{VK_NULL_HANDLE};
    VkPhysicalDevice getHandle() const{ return handle;}

    CDebugger * debugger;

    void setInstance(CInstance *instance);

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
