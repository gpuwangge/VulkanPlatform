#include "instance.h"

CInstance::CInstance(const std::vector<const char*> requiredValidationLayers){
    debugger = new CDebugger("../logs/instance.log");

    HERE_I_AM("CInstance Constructor");

    VkResult result = VK_SUCCESS;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VULKAN PLATFORM";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //First make sure required layer(s) are available
    if (enableValidationLayers) {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr); // see what layers are actually available:

        std::vector<VkLayerProperties> availableLayers(layerCount);
        result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        REPORT("vkEnumerateInstanceLayerProperties");
        DisplayLayers(availableLayers);

        for (const char* layerName : requiredValidationLayers) {// to find out validation layer in available layers
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    debugger->writeMSG("validation layers available!(%s)\n\n", layerName);
                    break;
                }
            }
        }

        if (result != VK_SUCCESS) throw std::runtime_error("validation layers requested, but not available!");
    }

    //Second set required extension(s)
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties((char *)nullptr, &extensionCount, (VkExtensionProperties *)nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    result = vkEnumerateInstanceExtensionProperties((char *)nullptr, &extensionCount, availableExtensions.data());
    REPORT("vkEnumerateInstanceExtensionProperties");
    DisplayExtensions(availableExtensions);

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = requiredValidationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    DisplayExtensions(extensions);

    //Third create instance
    result = vkCreateInstance(&createInfo, nullptr, &handle);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create instance!");

    REPORT("vkCreateInstance");

    Init02SetupDebugMessenger();
}

CInstance::~CInstance(){
    if (!debugger) delete debugger;
    //if (handle != VK_NULL_HANDLE) vkDestroyInstance(handle, nullptr);
}


void CInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

std::vector<const char*> CInstance::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkResult CInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void CInstance::Init02SetupDebugMessenger() {
    HERE_I_AM("Init02CreateDebugMessenger");

    VkResult result = VK_SUCCESS;

    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    result = CreateDebugUtilsMessengerEXT(handle, &createInfo, nullptr, &debugMessenger);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to set up debug messenger!");

    REPORT("CreateDebugUtilsMessengerEXT");
}

void CInstance::findAllPhysicalDevices(){
    HERE_I_AM("findAllPhysicalDevices");

    VkResult result = VK_SUCCESS;

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, (VkPhysicalDevice *)nullptr);
    
    if (result != VK_SUCCESS || deviceCount == 0) {
        debugger->writeMSG("Could not count the physical devices\n");
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    debugger->writeMSG("%d physical devices found.\n", deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, OUT devices.data());
    REPORT("vkEnumeratePhysicalDevices");

    if (result != VK_SUCCESS) {
        debugger->writeMSG("Could not enumerate the %d physical devices\n", deviceCount);
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    for (auto &physical_device : devices){
        //gpus.push_back(std::make_unique<CPhysicalDevice>(*this, physical_device));
        physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
        physicalDevices.back().get()->displayPhysicalDevices();
    }

}

std::unique_ptr<CPhysicalDevice>* CInstance::pickSuitablePhysicalDevice(VkSurfaceKHR surface, const std::vector<const char*>  requireDeviceExtensions, VkQueueFlagBits requiredQueueFamilies){
    HERE_I_AM("pickSuitablePhysicalDevice");

    if(physicalDevices.empty()){
        debugger->writeMSG("Error: No physical devices!\n");
        return nullptr;
    }

    for ( auto& phy_device : physicalDevices) {
        //Test Queue Families
        QueueFamilyIndices indices = phy_device->findQueueFamilies(surface);

        //Test Extension
        bool extensionsSupported = phy_device->checkDeviceExtensionSupport(requireDeviceExtensions);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            //Test SwapChain
            SwapChainSupportDetails swapChainSupport = phy_device->querySwapChainSupport(surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        if (indices.isComplete() && extensionsSupported && swapChainAdequate) {
            if(requiredQueueFamilies & VK_QUEUE_GRAPHICS_BIT) {
                debugger->writeMSG("Require VK_QUEUE_GRAPHICS_BIT\n"); debugger->flush();
                if(!indices.graphicsFamily.has_value()) return nullptr;
                debugger->writeMSG("Picked physical device index: %d\n", indices.graphicsFamily.value());debugger->flush();
            }
            if(requiredQueueFamilies & VK_QUEUE_COMPUTE_BIT) {
                debugger->writeMSG("Require VK_QUEUE_COMPUTE_BIT\n");debugger->flush();
                if(!indices.computeFamily.has_value()) return nullptr;
                debugger->writeMSG("Picked physical device index: %d\n", indices.computeFamily.value());debugger->flush();

            }
            if((requiredQueueFamilies & VK_QUEUE_COMPUTE_BIT) & VK_QUEUE_GRAPHICS_BIT){
                debugger->writeMSG("Require VK_QUEUE_COMPUTE_BIT & VK_QUEUE_GRAPHICS_BIT\n");debugger->flush();
                if(!indices.graphicsAndComputeFamily.has_value()) return nullptr;
                debugger->writeMSG("Picked physical device index: %d\n", indices.graphicsAndComputeFamily.value());debugger->flush();
            }

            //pickedPhysicalDevice = &phy_device;
            return &phy_device;
        }
    }

    //if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
    //}
    return nullptr;
}

void CInstance::DisplayLayers(std::vector<VkLayerProperties> &availableLayers){
    debugger->writeMSG("Available layers:\n");
    if(debugger->getVerbose()){
        for (const auto& layerProperties : availableLayers) {
            fprintf(debugger->FpDebug, "0x%08x  %5d  '%s'  '%s'\n",
                layerProperties.specVersion,
                layerProperties.implementationVersion,
                layerProperties.layerName,
                layerProperties.description);
        }
        debugger->writeMSG("\n");
    }
}

void CInstance::DisplayExtensions(std::vector<VkExtensionProperties> &availableExtensions){
    debugger->writeMSG("Available extensions:\n");
    if(debugger->getVerbose()){
        for (const auto& extensionsProperties : availableExtensions) {
            fprintf(debugger->FpDebug, "0x%08x  '%s'\n",
                extensionsProperties.specVersion,
                extensionsProperties.extensionName);
        }
        debugger->writeMSG("\n");
    }
}

void CInstance::DisplayExtensions(std::vector<const char*> &availableExtensions){
    debugger->writeMSG("Required extensions:\n");
    if(debugger->getVerbose()){
        for (const auto& extensionsProperties : availableExtensions) {
            fprintf(debugger->FpDebug, "'%s'\n",
                extensionsProperties);
        }
        debugger->writeMSG("\n");
    }
}

// void CInstance::DisplayRequiredExtensions(){
//     debugger->writeMSG("Required extensions:\n");
//     if(debugger->getVerbose()){
//         for (const auto& extensionsProperties : extensions) {
//             fprintf(debugger->FpDebug, "'%s'\n",
//                 extensionsProperties);
//         }
//         debugger->writeMSG("\n");
//     }
// }

    
