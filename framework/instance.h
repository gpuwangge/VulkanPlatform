#ifndef H_INSTANCE
#define H_INSTANCE


#include "debugger.h"
#include <vector>
#include <optional>
#include <iostream>
#include <memory>
#include <set>

#include <string.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define IN
#define OUT
#define INOUT

#include "physicalDevice.h"

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> graphicsAndComputeFamily;

	bool isComplete() {
		return graphicsAndComputeFamily.has_value() && graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class CPhysicalDevice; //Because instance.h uses CPhysicalDevice. To notify instance.h that CPhysicalDevice is a class (defined in other files)

class CInstance{
public:
    CInstance();

    ~CInstance();


    VkInstance handle{VK_NULL_HANDLE};
    VkInstance getHandle() const{ return handle;}

    VkDebugUtilsMessengerEXT debugMessenger;//02

    CDebugger * debugger;


    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    std::vector<const char*> getRequiredExtensions();

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void Init02SetupDebugMessenger();

    

    std::vector<std::unique_ptr<CPhysicalDevice>> physicalDevices;

    void queryGPUs();

    CPhysicalDevice &pick_suitable_gpu(VkSurfaceKHR surface);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
};


#endif