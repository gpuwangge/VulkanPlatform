#ifndef H_INSTANCE
#define H_INSTANCE


#include "debugger.h"
#include "common.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "physicalDevice.h"

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


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

    std::unique_ptr<CPhysicalDevice>* pick_suitable_gpu(VkSurfaceKHR surface);


};


#endif