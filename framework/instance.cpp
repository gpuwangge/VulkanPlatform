#include "instance.h"


const bool enableValidationLayers = false;
//VkInstance getHandle(){return handle;}
//VkInstance getHandle() const{ return handle;}

    CInstance::CInstance(){
        debugger = new CDebugger("instanceLog");

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

    CPhysicalDevice & CInstance::pick_suitable_gpu(VkSurfaceKHR surface){
        //(!physicalDevices.empty() && "No physical devices were found on the system.");

        //int whichDeviceSelected = -1;
        //int index = 0;

        for (const auto& phy_device : physicalDevices) {
            //测试Queue Families
            QueueFamilyIndices indices = findQueueFamilies(phy_device->getHandle(), surface);

            //测试Extension
            bool extensionsSupported = checkDeviceExtensionSupport(phy_device->getHandle());

            bool swapChainAdequate = false;
            if (extensionsSupported) {
                //测试SwapChain
                SwapChainSupportDetails swapChainSupport = querySwapChainSupport(phy_device->getHandle(), surface);
                swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            }
            if (indices.isComplete() && extensionsSupported && swapChainAdequate) {
                return *phy_device;
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

    QueueFamilyIndices CInstance::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        debugger->writeMSG("Found %d Queue Families:\n", queueFamilyCount);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

        int i = 0;
        for (const auto& queueFamilyProperty : queueFamilyProperties) {
            fprintf(debugger->FpDebug, "\t%d: Queue Family Count = %2d  ;   ", i, queueFamilyProperty.queueCount);
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)	fprintf(debugger->FpDebug, " Graphics");
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)	fprintf(debugger->FpDebug, " Compute");
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)	fprintf(debugger->FpDebug, " Transfer");
            fprintf(debugger->FpDebug, "\n");
            i++;
        }

        i = 0;
        for (const auto& queueFamilyProperty : queueFamilyProperties) {
            if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                indices.graphicsAndComputeFamily = i;
            }

            VkBool32 presentSupport = false;
            VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, OUT &presentSupport);
            REPORT("vkGetPhysicalDeviceSurfaceSupportKHR");

            if (presentSupport) {
                indices.presentFamily = i;
                fprintf(debugger->FpDebug, "This Surface is supported by the Graphics Queue\n");
            }
            else fprintf(debugger->FpDebug, "This Surface is not supported by the Graphics Queue\n");

            if (indices.isComplete()) break; //select the first suitable indices (which has graphics and present family)

            i++;
        }

        return indices;
    }

    bool CInstance::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails CInstance::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, OUT &details.capabilities);
        fprintf(debugger->FpDebug, "\nvkGetPhysicalDeviceSurfaceCapabilitiesKHR:\n");
        fprintf(debugger->FpDebug, "\tminImageCount = %d ; maxImageCount = %d\n", details.capabilities.minImageCount, details.capabilities.maxImageCount);
        fprintf(debugger->FpDebug, "\tcurrentExtent = %d x %d\n", details.capabilities.currentExtent.width, details.capabilities.currentExtent.height);
        fprintf(debugger->FpDebug, "\tminImageExtent = %d x %d\n", details.capabilities.minImageExtent.width, details.capabilities.minImageExtent.height);
        fprintf(debugger->FpDebug, "\tmaxImageExtent = %d x %d\n", details.capabilities.maxImageExtent.width, details.capabilities.maxImageExtent.height);
        fprintf(debugger->FpDebug, "\tmaxImageArrayLayers = %d\n", details.capabilities.maxImageArrayLayers);
        fprintf(debugger->FpDebug, "\tsupportedTransforms = 0x%04x\n", details.capabilities.supportedTransforms);
        fprintf(debugger->FpDebug, "\tcurrentTransform = 0x%04x\n", details.capabilities.currentTransform);
        fprintf(debugger->FpDebug, "\tsupportedCompositeAlpha = 0x%04x\n", details.capabilities.supportedCompositeAlpha);
        fprintf(debugger->FpDebug, "\tsupportedUsageFlags = 0x%04x\n", details.capabilities.supportedUsageFlags);


        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        debugger->writeMSG("\nFound %d Surface Formats:\n", formatCount);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }
        for (uint32_t i = 0; i < formatCount; i++) {
            fprintf(debugger->FpDebug, "%3d:     %4d     %12d", i, details.formats[i].format, details.formats[i].colorSpace); \
                if (details.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_SRGB_NONLINEAR_KHR\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_DCI_P3_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DCI_P3_LINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_BT709_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_BT709_LINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_BT709_NONLINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_BT709_NONLINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_BT2020_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_BT2020_LINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_HDR10_ST2084_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_DOLBYVISION_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_DOLBYVISION_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_HDR10_HLG_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_HDR10_HLG_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT)			fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_ADOBERGB_LINEAR_EXT\n");
            if (details.formats[i].colorSpace == VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT)		fprintf(debugger->FpDebug, "\tVK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT\n");

        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        debugger->writeMSG("\nFound %d Present Modes:\n", presentModeCount);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        for (uint32_t i = 0; i < presentModeCount; i++) {
            fprintf(debugger->FpDebug, "%3d:     %4d", i, details.presentModes[i]);
            if (details.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_IMMEDIATE_KHR\n");
            if (details.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_MAILBOX_KHR\n");
            if (details.presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)			fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_FIFO_KHR\n");
            if (details.presentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)		fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_FIFO_RELAXED_KHR\n");
            if (details.presentModes[i] == VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR)	fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR\n");
            if (details.presentModes[i] == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR)	fprintf(debugger->FpDebug, "\tVK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR\n");
        }
        fprintf(stderr, "\n");

        return details;
    }
