#include "application.h"

CApplication::CApplication(){
    debugger = new CDebugger("appLog.txt");
}

void CApplication::run(){
    prepareGLFW();
    initVulkan();
    mainLoop();
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<CApplication*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}

void CApplication::prepareGLFW(){
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Appliciation", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

		//glfwSetKeyCallback(window, GLFWKeyboard);
		//glfwSetCursorPosCallback(window, GLFWMouseMotion);
		//glfwSetMouseButtonCallback(window, GLFWMouseButton);
}


void CApplication::createGLFWSurface() {
    if (glfwCreateWindowSurface(instance->getHandle(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}









// void CApplication::Init03PickPhysicalDevice() {
   
// }
    
void CApplication::Init04CreateLogicalDevice() {
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
}

int CApplication::FindMemoryByFlagAndType(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits) {
    VkPhysicalDeviceMemoryProperties	vpdmp;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, OUT &vpdmp);
    for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
        VkMemoryType vmt = vpdmp.memoryTypes[i];
        VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
        if ((memoryTypeBits & (1 << i)) != 0) {
            if ((vmpf & memoryFlagBits) != 0){
                fprintf(debugger->FpDebug, "Found given memory flag (0x%08x) and type (0x%08x): i = %d\n", memoryFlagBits, memoryTypeBits, i);
                return i;
            }
        }
    }

    fprintf(debugger->FpDebug, "Could not find given memory flag (0x%08x) and type (0x%08x)\n", memoryFlagBits, memoryTypeBits);
    throw  std::runtime_error("Could not find given memory flag and type");
}

int CApplication::FindMemoryThatIsHostVisible(uint32_t memoryTypeBits) {
    return FindMemoryByFlagAndType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryTypeBits);
}

VkResult CApplication::InitDataBufferHelper(VkDeviceSize size, VkBufferUsageFlags usage, OUT MyBuffer * pMyBuffer) {
    //HERE_I_AM("Init05DataBuffer");
    
    //Step1:
    VkResult result = VK_SUCCESS;

    VkBufferCreateInfo  vbci;
    vbci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vbci.pNext = nullptr;
    vbci.flags = 0;
    vbci.size = size;
    vbci.usage = usage;
#ifdef CHOICES
    VK_USAGE_TRANSFER_SRC_BIT
        VK_USAGE_TRANSFER_DST_BIT
        VK_USAGE_UNIFORM_TEXEL_BUFFER_BIT
        VK_USAGE_STORAGE_TEXEL_BUFFER_BIT
        VK_USAGE_UNIFORM_BUFFER_BIT
        VK_USAGE_STORAGE_BUFFER_BIT
        VK_USAGE_INDEX_BUFFER_BIT
        VK_USAGE_VERTEX_BUFFER_BIT
        VK_USAGE_INDIRECT_BUFFER_BIT
#endif
    vbci.queueFamilyIndexCount = 0;
    vbci.pQueueFamilyIndices = (const uint32_t *)nullptr;
    vbci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	// can only use CONCURRENT if .queueFamilyIndexCount > 0
#ifdef CHOICES
    VK_SHARING_MODE_EXCLUSIVE
        VK_SHARING_MODE_CONCURRENT
#endif

    pMyBuffer->size = size;
    result = vkCreateBuffer(logicalDevice, IN &vbci, PALLOCATOR, OUT &pMyBuffer->buffer);
    REPORT("vkCreateBuffer");

    //Step 2:
    VkMemoryRequirements			vmr;
    vkGetBufferMemoryRequirements(logicalDevice, IN pMyBuffer->buffer, OUT &vmr);		// fills vmr
    //if (Verbose){
    fprintf(debugger->FpDebug, "Buffer vmr.size = %lld\n", vmr.size);
    fprintf(debugger->FpDebug, "Buffer vmr.alignment = %lld\n", vmr.alignment);
    fprintf(debugger->FpDebug, "Buffer vmr.memoryTypeBits = 0x%08x\n", vmr.memoryTypeBits);
    fflush(debugger->FpDebug);
    //}

    VkMemoryAllocateInfo			vmai;
    vmai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vmai.pNext = nullptr;
    vmai.allocationSize = vmr.size;
    vmai.memoryTypeIndex = FindMemoryThatIsHostVisible(vmr.memoryTypeBits);

    VkDeviceMemory				vdm;
    result = vkAllocateMemory(logicalDevice, IN &vmai, PALLOCATOR, OUT &vdm);
    REPORT("vkAllocateMemory");
    pMyBuffer->deviceMemory = vdm;

    //Step 3:
    result = vkBindBufferMemory(logicalDevice, pMyBuffer->buffer, IN vdm, 0);		// 0 is the offset
    REPORT("vkBindBufferMemory");

    return result;
}

VkResult CApplication::FillDataBufferHelper(IN MyBuffer myBuffer, IN void * data) {
    // the size of the data had better match the size that was used to Init the buffer!

    //Step 4:
    void * pGpuMemory;
    vkMapMemory(logicalDevice, IN myBuffer.deviceMemory, 0, VK_WHOLE_SIZE, 0, &pGpuMemory);	// 0 and 0 are offset and flags
    memcpy(pGpuMemory, data, (size_t)myBuffer.size);
    vkUnmapMemory(logicalDevice, IN myBuffer.deviceMemory);
    return VK_SUCCESS;

    // the way shown here makes it happen immediately
    //
    // but, we could use vkCmdUpdateBuffer( CommandBuffer, myBuffer.buffer, 0, myBuffer.size, data );
    // instead, except that this requires use of the CommandBuffer
    // which might not be so bad since we are using the CommandBuffer at that moment to draw anyway
}

void CApplication::Init05CreateVertexBuffer() {
    HERE_I_AM("Init05CreateVertexBuffer");
    VkDeviceSize bufferSize = sizeof(vertices3D[0]) * vertices3D.size();

    VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &vertexDataBuffer);//为vertexDataBuffer申请bufferSize(由vertices3D确定)大小的memory
    REPORT("InitVertexDataBuffer");
    FillDataBufferHelper(vertexDataBuffer, (void *)(vertices3D.data()));//把vertices3D的内容拷贝到vertexDataBuffer里
}

void CApplication::Init05CreateIndexBuffer() {
    HERE_I_AM("Init05CreateIndexBuffer");
    VkDeviceSize bufferSize = sizeof(indices3D[0]) * indices3D.size();

    VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &indexDataBuffer);
    REPORT("InitIndexDataBuffer");
    FillDataBufferHelper(indexDataBuffer, (void *)(indices3D.data()));
}

void CApplication::Init05CreateUniformBuffers(std::vector<MyBuffer> &_uniformBuffers, std::vector<void*> &_uniformBuffersMapped, VkBufferUsageFlags usage, VkDeviceSize bufferSize) {
    HERE_I_AM("Init05CreateUniformBuffers");

    _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = InitDataBufferHelper(bufferSize, usage, &_uniformBuffers[i]);
        REPORT("Init05CreateUniformBuffers");

        vkMapMemory(logicalDevice, _uniformBuffers[i].deviceMemory, 0, bufferSize, 0, &_uniformBuffersMapped[i]);
    }
}

void CApplication::Init06CreateCommandPool() {
    HERE_I_AM("Init06CommandPools");

    VkResult result = VK_SUCCESS;

    QueueFamilyIndices queueFamilyIndices = instance->findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    //poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();//find a queue family that does graphics
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();//graphics command buffer和compute command buffer都需要这个command pool，所以需要graphics and compute family

    result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics command pool!");
    REPORT("vkCreateCommandPool -- Graphics");
}

void CApplication::Init06CreateCommandBuffers() {
    HERE_I_AM("Init06CommandBuffers");

    VkResult result = VK_SUCCESS;

    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    result = vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data());//graphcis recordCommandBuffer需要用这个buffer： recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate command buffers!");
    REPORT("vkAllocateCommandBuffers");
}

VkSurfaceFormatKHR CApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR CApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D CApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkImageView CApplication::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;

    //Components可以调配texture上各个channel的颜色
    //VK_COMPONENT_SWIZZLE_IDENTITY is default value
    //VK_COMPONENT_SWIZZLE_ZERO会使该channel的分量归零
    //VK_COMPONENT_SWIZZLE_R把该channel赋值为r channel的值
    //VK_COMPONENT_SWIZZLE_ONE把该channel的值赋为最大
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void CApplication::Init08CreateSwapChain() {
    HERE_I_AM("Init08Swapchain");

    VkResult result = VK_SUCCESS;

    SwapChainSupportDetails swapChainSupport = instance->querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = instance->findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    //生成swapChain
    result = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create swap chain!");
    REPORT("vkCreateSwapchainKHR");

    result = vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
    REPORT("vkGetSwapchainImagesKHR - 0 (Get imageCount)");
    swapChainImages.resize(imageCount);
    result = vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());
    REPORT("vkGetSwapchainImagesKHR - 1");

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    // present views for the double-buffering:
    swapChainImageViews.resize(swapChainImages.size());

    //生成swapChainImageViews
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        REPORT("vkCreateImageView");
    }
}

VkFormat CApplication::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat CApplication::findDepthFormat() {
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}



static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void CApplication::Init12SpirvShader(std::string filename, VkShaderModule * pShaderModule){
    HERE_I_AM("Init12SpirvShader");

    auto shaderCode = readFile(filename.c_str());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(logicalDevice, &createInfo, PALLOCATOR, pShaderModule);
    REPORT("vkCreateShaderModule");
    debugger->writeMSG("Shader Module '%s' successfully loaded\n", filename.c_str());

}

void CApplication::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    //computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    //computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        //if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            //vkCreateFence(logicalDevice, &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            //throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        //}
    }
}

void CApplication::initVulkan(){
    instance = std::make_unique<CInstance>();
    
    createGLFWSurface();//need instance

    //Init03PickPhysicalDevice();
    instance->queryGPUs();
    physicalDevice = (instance->pick_suitable_gpu(surface)).getHandle();

    Init04CreateLogicalDevice();

    Init05CreateVertexBuffer();
    Init05CreateIndexBuffer();
    Init05CreateUniformBuffers(uniformBuffers, uniformBuffersMapped, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject));

	Init06CreateCommandPool();
	Init06CreateCommandBuffers();

    Init08CreateSwapChain();

    Init10CreateRenderPass();

    Init11CreateFramebuffers();

    Init12SpirvShader(vertexShaderPath, &vertShaderModule[0]);
	Init12SpirvShader(fragmentShaderPath, &fragShaderModule[0]);

    Init13CreateDescriptorPool(descriptorPool[0], (PipelineType)0);
    Init13CreateDescriptorSetLayout(descriptorSetLayout[0], (PipelineType)0);
    Init13CreateDescriptorSets(descriptorPool[0], descriptorSetLayout[0], descriptorSets[0], (PipelineType)0);

    Init14CreateGraphicsPipeline(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, vertShaderModule[0], fragShaderModule[0], descriptorSetLayout[0], pipelineLayout[0], graphicsPipeline[0], (PipelineType)0);

    createSyncObjects();
}

void CApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //Step1
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    // if (bEnableDepthTest) {
    //     clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    //     clearValues[1].depthStencil = { 1.0f, 0 };
    //     renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    //     renderPassInfo.pClearValues = clearValues.data();
    // }
    // else {
        clearValues[0].color = { {  0.0f, 0.0f, 0.0f, 1.0f  } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = clearValues.data();
    //}

    //Step2
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Step3
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline[0]);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    //Step4
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    //compute piepline的结果在shaderStorageBuffers_compute里面，而其他pipeline使用vertexDataBuffer和indexDataBuffer
    // if (pipelineType == PIPELINE_COMPUTE) {
    //     VkDeviceSize offsets[] = { 0 };
    //     vkCmdBindVertexBuffers(commandBuffer, 0, 1, &shaderStorageBuffers_compute[currentFrame].buffer, offsets);

    //     //Step5
    //     vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);
    // }
    // else {
        VkBuffer vertexBuffers[] = { vertexDataBuffer.buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffer, indexDataBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        //Descriptor Step 5
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout[0], 0, 1, &descriptorSets[0][currentFrame], 0, nullptr);

        //Step5
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices3D.size()), 1, 0, 0, 0);
    //}

    //Step6
    vkCmdEndRenderPass(commandBuffer);

    //Step7
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}

void CApplication::drawFrame() {
    //printf("app drawFrame...\n");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //=======================Compute submission========================
    // if (pipelineType == PIPELINE_COMPUTE) {
    //     vkWaitForFences(logicalDevice, 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //     updateUniformBuffer_compute(currentFrame);

    //     vkResetFences(logicalDevice, 1, &computeInFlightFences[currentFrame]);

    //     vkResetCommandBuffer(commandBuffers_compute[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    //     recordComputeCommandBuffer(commandBuffers_compute[currentFrame]); //Dispatch in this function

    //     submitInfo.commandBufferCount = 1;
    //     submitInfo.pCommandBuffers = &commandBuffers_compute[currentFrame];
    //     submitInfo.signalSemaphoreCount = 1;
    //     submitInfo.pSignalSemaphores = &computeFinishedSemaphores[currentFrame];

    //     if (vkQueueSubmit(computeQueue, 1, &submitInfo, computeInFlightFences[currentFrame]) != VK_SUCCESS) {
    //         throw std::runtime_error("failed to submit compute command buffer!");
    //     };
    // }
    //=======================Graphics submission========================
    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //recreateSwapChain();//TODO
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    //if (pipelineType != PIPELINE_COMPUTE) {
        //updateUniformBuffer(currentFrame); //目前pariticle system不需要更新MVP,移动此函数到update()
    //}

    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //NTODO: present and Semaphore
    VkPresentInfoKHR presentInfo{};
    // if (pipelineType == PIPELINE_COMPUTE) {
    //     VkSemaphore waitSemaphores[] = { computeFinishedSemaphores[currentFrame], imageAvailableSemaphores[currentFrame] };
    //     VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    //     submitInfo = {};
    //     submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //     submitInfo.waitSemaphoreCount = 2;
    //     submitInfo.pWaitSemaphores = waitSemaphores;
    //     submitInfo.pWaitDstStageMask = waitStages;
    //     submitInfo.commandBufferCount = 1;
    //     submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    //     submitInfo.signalSemaphoreCount = 1;
    //     submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

    //     presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
    // }
    // else {
        //VkSemaphore waitSemaphores[] = { computeFinishedSemaphores[currentFrame],  imageAvailableSemaphores[currentFrame] };
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        presentInfo.pWaitSemaphores = signalSemaphores;
    //}

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        debugger->writeMSG("Failed to submit draw command buffer! CurrentFrame: %d\n", currentFrame);
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;


    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        //recreateSwapChain();//TODO
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

void CApplication::updateUniformBuffer(uint32_t currentFrame, float durationTime) {

}

void CApplication::update(){
    //printf("app update...\n");

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float durationTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    float deltaTime = (lastTime - currentTime).count();
    lastTime = currentTime;

    // We want to animate the particle system using the last frames time to get smooth, frame-rate independent animation
    //double currentTime = glfwGetTime();
    //elapsedTime = (currentTime - lastTime) * 1000.0;//这个值会用来update deltaTime
    //lastTime = currentTime.count();

    //camera.update(deltaTime);

    updateUniformBuffer(currentFrame, durationTime);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CApplication::mainLoop(){
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
            update();
            drawFrame();
			//if (NeedToExit) break;
		}

		vkDeviceWaitIdle(logicalDevice);
}

void CApplication::cleanupSwapChain() {
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

void CApplication::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

CApplication::~CApplication(){
    //clean up
    HERE_I_AM("Clean up application");

    cleanupSwapChain();

    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

    //vkDestroyPipeline(logicalDevice, pipeline_compute, nullptr);
    //vkDestroyPipelineLayout(logicalDevice, pipelineLayout_compute, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(logicalDevice, uniformBuffers[i].buffer, nullptr);
        vkFreeMemory(logicalDevice, uniformBuffers[i].deviceMemory, nullptr);
    }

    for (int i = 0; i < 1; i++) {
        vkDestroyDescriptorPool(logicalDevice, descriptorPool[i], nullptr);
        vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout[i], nullptr);
        vkDestroyPipeline(logicalDevice, graphicsPipeline[i], nullptr);
        vkDestroyPipelineLayout(logicalDevice, pipelineLayout[i], nullptr);

        //vkDestroyImage(logicalDevice, textureImageBuffer[i].image, nullptr);
        //vkFreeMemory(logicalDevice, textureImageBuffer[i].deviceMemory, nullptr);
        //vkDestroyImageView(logicalDevice, textureImageView[i], nullptr);
    }
    // for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {
    //     vkDestroyImage(logicalDevice, textureImageBuffers_mipmap[i].image, nullptr);
    //     vkFreeMemory(logicalDevice, textureImageBuffers_mipmap[i].deviceMemory, nullptr);
    // }
    //vkDestroyImageView(logicalDevice, colorImageView_msaa, nullptr);
    //vkDestroyImage(logicalDevice, colorImageBuffer_msaa.image, nullptr);
    //vkFreeMemory(logicalDevice, colorImageBuffer_msaa.deviceMemory, nullptr);

    //vkDestroySampler(logicalDevice, textureSampler, nullptr);

    //vkDestroyImage(logicalDevice, depthImageBuffer.image, nullptr);
    //vkFreeMemory(logicalDevice, depthImageBuffer.deviceMemory, nullptr);
    //vkDestroyImageView(logicalDevice, depthImageView, nullptr);

    vkDestroyBuffer(logicalDevice, indexDataBuffer.buffer, nullptr);
    vkFreeMemory(logicalDevice, indexDataBuffer.deviceMemory, nullptr);

    vkDestroyBuffer(logicalDevice, vertexDataBuffer.buffer, nullptr);
    vkFreeMemory(logicalDevice, vertexDataBuffer.deviceMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
        //vkDestroySemaphore(logicalDevice, computeFinishedSemaphores[i], nullptr);
        vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
        //vkDestroyFence(logicalDevice, computeInFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

    vkDestroyDevice(logicalDevice, nullptr);

/*TODO
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);*/

    glfwDestroyWindow(window);

    glfwTerminate();

    delete debugger;
}