#include "instance.h"
#include "logicalDevice.hpp"

class CPhysicalDevice{
public:
    //CInstance &instance;
    //CPhysicalDevice(CInstance &instance, VkPhysicalDevice physical_device) : instance{instance}, handle{physical_device} {
    CPhysicalDevice( VkPhysicalDevice physical_device) : handle{physical_device} {
        printf("physical\n");
        debugger = new CDebugger("physicalDeviceLog");

        //CInstance ins();

        HERE_I_AM("Init03PhysicalDeviceAndGetQueueFamilyProperties");

        VkResult result = VK_SUCCESS;

        /*
        uint32_t deviceCount = 0;
        result = vkEnumeratePhysicalDevices(instance->getHandle(), OUT &deviceCount, (VkPhysicalDevice *)nullptr);
        REPORT("vkEnumeratePhysicalDevices - 1");

        if (result != VK_SUCCESS || deviceCount == 0) {
            debugger->writeMSG("Could not count the physical devices\n");
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        debugger->writeMSG("%d physical devices found.\nn", deviceCount);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        result = vkEnumeratePhysicalDevices(instance->getHandle(), OUT &deviceCount, OUT devices.data());
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
        }*/

        //选择物理设备
        /*
        int whichDeviceSelected = -1;
        int index = 0;
        for (const auto& device : devices) {
            //测试Queue Families
            QueueFamilyIndices indices = findQueueFamilies(device);

            //测试Extension
            bool extensionsSupported = checkDeviceExtensionSupport(device);

            bool swapChainAdequate = false;
            if (extensionsSupported) {
                //测试SwapChain
                SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
                swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            }
            if (indices.isComplete() && extensionsSupported && swapChainAdequate) {
                physicalDevice = device;
                whichDeviceSelected = index;
                msaaSamples = getMaxUsableSampleCount_msaa();//MSAA 1 VK_SAMPLE_COUNT_1_BIT
                break;
            }
            index++;
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }*/


/*TODO
        VkPhysicalDeviceProperties	PhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, OUT &PhysicalDeviceProperties);
        fprintf(debugger->FpDebug, "\nDevice #%d selected ('%s')\n", whichDeviceSelected, PhysicalDeviceProperties.deviceName);

        //显示选中的PhysicalDevice Features
        VkPhysicalDeviceFeatures	PhysicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(IN physicalDevice, OUT &PhysicalDeviceFeatures);

        fprintf(debugger->FpDebug, "\nPhysical Device Features:\n");
        fprintf(debugger->FpDebug, "geometryShader = %2d\n", PhysicalDeviceFeatures.geometryShader);
        fprintf(debugger->FpDebug, "tessellationShader = %2d\n", PhysicalDeviceFeatures.tessellationShader);
        fprintf(debugger->FpDebug, "multiDrawIndirect = %2d\n", PhysicalDeviceFeatures.multiDrawIndirect);
        fprintf(debugger->FpDebug, "wideLines = %2d\n", PhysicalDeviceFeatures.wideLines);
        fprintf(debugger->FpDebug, "largePoints = %2d\n", PhysicalDeviceFeatures.largePoints);
        fprintf(debugger->FpDebug, "multiViewport = %2d\n", PhysicalDeviceFeatures.multiViewport);
        fprintf(debugger->FpDebug, "occlusionQueryPrecise = %2d\n", PhysicalDeviceFeatures.occlusionQueryPrecise);
        fprintf(debugger->FpDebug, "pipelineStatisticsQuery = %2d\n", PhysicalDeviceFeatures.pipelineStatisticsQuery);
        fprintf(debugger->FpDebug, "shaderFloat64 = %2d\n", PhysicalDeviceFeatures.shaderFloat64);
        fprintf(debugger->FpDebug, "shaderInt64 = %2d\n", PhysicalDeviceFeatures.shaderInt64);
        fprintf(debugger->FpDebug, "shaderInt16 = %2d\n", PhysicalDeviceFeatures.shaderInt16);

        VkFormatProperties					vfp;
        fprintf(debugger->FpDebug, "\nImage Formats Checked:\n");
        vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_R32G32B32A32_SFLOAT, &vfp);
        fprintf(debugger->FpDebug, "Format VK_FORMAT_R32G32B32A32_SFLOAT: 0x%08x 0x%08x  0x%08x\n",
            vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
        vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_R8G8B8A8_UNORM, &vfp);
        fprintf(debugger->FpDebug, "Format VK_FORMAT_R8G8B8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
            vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
        vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_B8G8R8A8_UNORM, &vfp);
        fprintf(debugger->FpDebug, "Format VK_FORMAT_B8G8R8A8_UNORM: 0x%08x 0x%08x  0x%08x\n",
            vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);
        vkGetPhysicalDeviceFormatProperties(physicalDevice, IN VK_FORMAT_B8G8R8A8_SRGB, &vfp);
        fprintf(debugger->FpDebug, "Format VK_FORMAT_B8G8R8A8_SRGB: 0x%08x 0x%08x  0x%08x\n",
            vfp.linearTilingFeatures, vfp.optimalTilingFeatures, vfp.bufferFeatures);

        VkPhysicalDeviceMemoryProperties			vpdmp;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, OUT &vpdmp);
        fprintf(debugger->FpDebug, "\n%d Memory Types:\n", vpdmp.memoryTypeCount);
        for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
            VkMemoryType vmt = vpdmp.memoryTypes[i];
            VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
            fprintf(debugger->FpDebug, "Memory %2d: ", i);
            if ((vmpf & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)	fprintf(debugger->FpDebug, " DeviceLocal");
            if ((vmpf & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)	fprintf(debugger->FpDebug, " HostVisible");
            if ((vmpf & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0)	fprintf(debugger->FpDebug, " HostCoherent");
            if ((vmpf & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0)	fprintf(debugger->FpDebug, " HostCached");
            if ((vmpf & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0)	fprintf(debugger->FpDebug, " LazilyAllocated");
            fprintf(debugger->FpDebug, "\n");
        }

        fprintf(debugger->FpDebug, "\n%d Memory Heaps:\n", vpdmp.memoryHeapCount);
        for (unsigned int i = 0; i < vpdmp.memoryHeapCount; i++) {
            fprintf(debugger->FpDebug, "Heap %d: ", i);
            VkMemoryHeap vmh = vpdmp.memoryHeaps[i];
            fprintf(debugger->FpDebug, " size = 0x%08lx", (unsigned long int)vmh.size);
            if ((vmh.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)	fprintf(debugger->FpDebug, " DeviceLocal");
            if ((vmh.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT) != 0)	fprintf(debugger->FpDebug, " MultiInstance");
            fprintf(debugger->FpDebug, "\n");
        }
        fprintf(debugger->FpDebug, "\n");


        // see what device layers are available:
        uint32_t layerCount;
        vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, (VkLayerProperties *)nullptr);
        VkLayerProperties * deviceLayers = new VkLayerProperties[layerCount];
        result = vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, deviceLayers);
        REPORT("vkEnumerateDeviceLayerProperties");
        fprintf(debugger->FpDebug, "\n%d physical device layers enumerated:\n", layerCount);
        for (unsigned int i = 0; i < layerCount; i++) {
            fprintf(debugger->FpDebug, "0x%08x  %2d  '%s'  '%s'\n",
                deviceLayers[i].specVersion,
                deviceLayers[i].implementationVersion,
                deviceLayers[i].layerName,
                deviceLayers[i].description);

            // see what device extensions are available:
            //这里列举的是跟某个layer相关的extension
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, deviceLayers[i].layerName, &extensionCount, (VkExtensionProperties *)nullptr);
            VkExtensionProperties * deviceExtensions = new VkExtensionProperties[extensionCount];
            result = vkEnumerateDeviceExtensionProperties(physicalDevice, deviceLayers[i].layerName, &extensionCount, deviceExtensions);
            REPORT("vkEnumerateDeviceExtensionProperties");

            fprintf(debugger->FpDebug, "\t%d device extensions enumerated for '%s':\n", extensionCount, deviceLayers[i].layerName);
            for (unsigned int ii = 0; ii < extensionCount; ii++) {
                fprintf(debugger->FpDebug, "\t0x%08x  '%s'\n",
                    deviceExtensions[ii].specVersion,
                    deviceExtensions[ii].extensionName);
            }
            fprintf(debugger->FpDebug, "\n");
        }
        delete[] deviceLayers;*/
    }

    ~CPhysicalDevice(){
        if (!debugger) delete debugger;
    }

    VkPhysicalDevice handle{VK_NULL_HANDLE};
    VkPhysicalDevice getHandle() const{ return handle;}

    CDebugger * debugger;

    std::vector<std::unique_ptr<CLogicalDevice>> logicalDevices;

    void createLogicalDevices(){
        //physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
        logicalDevices.push_back(std::make_unique<CLogicalDevice>());

        /*
        HERE_I_AM("Init04LogicalDeviceAndQueue");

        VkResult result = VK_SUCCESS;

        //QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        QueueFamilyIndices indices = instance->findQueueFamilies(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        //建立logicalDevice
        result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
        if (result != VK_SUCCESS) throw std::runtime_error("failed to create logical device!");
        REPORT("vkCreateLogicalDevice");

        //把PhysicalDevice的Queue Familily属性指定到logicalDevice里对应的queue里
        vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue); //graphics queue use physical device's family 0 
        vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue); //present queue use the same family
                                                                                            //A physical device has several family, queue is pointing to one of the families
        vkGetDeviceQueue(logicalDevice, indices.graphicsAndComputeFamily.value(), 0, &computeQueue);
        */
    }


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