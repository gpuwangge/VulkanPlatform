#include "instance.h"


    CInstance::CInstance(){
        debugger = new CDebugger("instanceLog.txt");

		HERE_I_AM("Init01Instance");

		VkResult result = VK_SUCCESS;

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
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

			debugger->writeMSG("Available layers:\n");
			for (const auto& layerProperties : availableLayers) {
				fprintf(debugger->FpDebug, "0x%08x  %5d  '%s'  '%s'\n",
					layerProperties.specVersion,
					layerProperties.implementationVersion,
					layerProperties.layerName,
					layerProperties.description);
			}

			for (const char* layerName : validationLayers) {// to find out validation layer in available layers
				bool layerFound = false;

				for (const auto& layerProperties : availableLayers) {
					if (strcmp(layerName, layerProperties.layerName) == 0) {
						layerFound = true;
						debugger->writeMSG("validation layers available!(%s)\n", layerName);
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
		debugger->writeMSG("Available extensions:\n");
		for (const auto& extensionsProperties : availableExtensions) {
			fprintf(debugger->FpDebug, "0x%08x  '%s'\n",
				extensionsProperties.specVersion,
				extensionsProperties.extensionName);
		}

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

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
		debugger->writeMSG("Required extensions:\n");
		for (const auto& extensionsProperties : extensions) {
			fprintf(debugger->FpDebug, "'%s'\n",
				extensionsProperties);
		}

		//Third create instance
		result = vkCreateInstance(&createInfo, nullptr, &handle);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create instance!");

		REPORT("vkCreateInstance");

        Init02SetupDebugMessenger();
    }

    CInstance::~CInstance(){
        if (!debugger) delete debugger;
        if (handle != VK_NULL_HANDLE) vkDestroyInstance(handle, nullptr);
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

    void CInstance::queryGPUs(){
        VkResult result = VK_SUCCESS;

        uint32_t deviceCount = 0;
        result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, (VkPhysicalDevice *)nullptr);
        
        if (result != VK_SUCCESS || deviceCount == 0) {
            debugger->writeMSG("Could not count the physical devices\n");
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        debugger->writeMSG("%d physical devices found.\nn", deviceCount);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, OUT devices.data());
        REPORT("vkEnumeratePhysicalDevices - 2");

        if (result != VK_SUCCESS) {
            debugger->writeMSG("Could not enumerate the %d physical devices\n", deviceCount);
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        for (unsigned int i = 0; i < deviceCount; i++) {
            VkPhysicalDeviceProperties	PhysicalDeviceProperties;
            vkGetPhysicalDeviceProperties(IN devices[i], OUT &PhysicalDeviceProperties);

            fprintf(debugger->FpDebug, "\nDevice %2d:\n", i);
            fprintf(debugger->FpDebug, "\tAPI version: %d\n", PhysicalDeviceProperties.apiVersion);
            fprintf(debugger->FpDebug, "\tDriver version: %d\n", PhysicalDeviceProperties.apiVersion);
            fprintf(debugger->FpDebug, "\tVendor ID: 0x%04x\n", PhysicalDeviceProperties.vendorID);
            fprintf(debugger->FpDebug, "\tDevice ID: 0x%04x\n", PhysicalDeviceProperties.deviceID);
            fprintf(debugger->FpDebug, "\tPhysical Device Type: %d =", PhysicalDeviceProperties.deviceType);
            if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)	fprintf(debugger->FpDebug, " (Discrete GPU)\n");
            if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)	fprintf(debugger->FpDebug, " (Integrated GPU)\n");
            if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)	fprintf(debugger->FpDebug, " (Virtual GPU)\n");
            if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)		fprintf(debugger->FpDebug, " (CPU)\n");
            fprintf(debugger->FpDebug, "\tDevice Name: %s\n", PhysicalDeviceProperties.deviceName);
            fprintf(debugger->FpDebug, "\tPipeline Cache Size: %d\n", PhysicalDeviceProperties.pipelineCacheUUID[0]);
        }

        for (auto &physical_device : devices)
		    //gpus.push_back(std::make_unique<CPhysicalDevice>(*this, physical_device));
            physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
	
    }

    std::unique_ptr<CPhysicalDevice>* CInstance::pick_suitable_gpu(VkSurfaceKHR surface){
        //(!physicalDevices.empty() && "No physical devices were found on the system.");

        //int whichDeviceSelected = -1;
        //int index = 0;

        for ( auto& phy_device : physicalDevices) {
            //测试Queue Families
            QueueFamilyIndices indices = phy_device->findQueueFamilies(surface);

            //测试Extension
            bool extensionsSupported = phy_device->checkDeviceExtensionSupport();

            bool swapChainAdequate = false;
            if (extensionsSupported) {
                //测试SwapChain
                SwapChainSupportDetails swapChainSupport = phy_device->querySwapChainSupport(surface);
                swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            }
            if (indices.isComplete() && extensionsSupported && swapChainAdequate) {
                return &phy_device;
                //physicalDevice = device;
                //whichDeviceSelected = index;
                //msaaSamples = getMaxUsableSampleCount_msaa();//MSAA 1 VK_SAMPLE_COUNT_1_BIT
                //break;
            }
            //index++;
        }

        //if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
        //}
    }

    
