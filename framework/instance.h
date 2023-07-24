#ifndef H_INSTANCE
#define H_INSTANCE


#include "debugger.h"
#include "common.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "physicalDevice.h"




class CInstance{
public:
    CInstance(const std::vector<const char*> requiredValidationLayers);

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
    std::unique_ptr<CPhysicalDevice>* pickedPhysicalDevice;

    void findAllPhysicalDevices();

    //std::unique_ptr<CPhysicalDevice>* pick_suitable_gpu(VkSurfaceKHR surface);
    bool pickSuitablePhysicalDevice(VkSurfaceKHR surface, const std::vector<const char*>  requireDeviceExtensions);


};


#endif