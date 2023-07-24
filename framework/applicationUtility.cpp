#include "application.h"


/**************
Memory Management Utility Functions
************/
int CApplication::FindMemoryByFlagAndType(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits) {
    VkPhysicalDeviceMemoryProperties	vpdmp;
    vkGetPhysicalDeviceMemoryProperties(instance->pickedPhysicalDevice->get()->getHandle(), OUT &vpdmp);
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
    result = vkCreateBuffer(LOGICAL_DEVICE, IN &vbci, PALLOCATOR, OUT &pMyBuffer->buffer);
    REPORT("vkCreateBuffer");

    //Step 2:
    VkMemoryRequirements			vmr;
    vkGetBufferMemoryRequirements(LOGICAL_DEVICE, IN pMyBuffer->buffer, OUT &vmr);		// fills vmr
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
    result = vkAllocateMemory(LOGICAL_DEVICE, IN &vmai, PALLOCATOR, OUT &vdm);
    REPORT("vkAllocateMemory");
    pMyBuffer->deviceMemory = vdm;

    //Step 3:
    result = vkBindBufferMemory(LOGICAL_DEVICE, pMyBuffer->buffer, IN vdm, 0);		// 0 is the offset
    REPORT("vkBindBufferMemory");

    return result;
}

VkResult CApplication::FillDataBufferHelper(IN MyBuffer myBuffer, IN void * data) {
    // the size of the data had better match the size that was used to Init the buffer!

    //Step 4:
    void * pGpuMemory;
    vkMapMemory(LOGICAL_DEVICE, IN myBuffer.deviceMemory, 0, VK_WHOLE_SIZE, 0, &pGpuMemory);	// 0 and 0 are offset and flags
    memcpy(pGpuMemory, data, (size_t)myBuffer.size);
    vkUnmapMemory(LOGICAL_DEVICE, IN myBuffer.deviceMemory);
    return VK_SUCCESS;

    // the way shown here makes it happen immediately
    //
    // but, we could use vkCmdUpdateBuffer( CommandBuffer, myBuffer.buffer, 0, myBuffer.size, data );
    // instead, except that this requires use of the CommandBuffer
    // which might not be so bad since we are using the CommandBuffer at that moment to draw anyway
}

/**************
Swap Chain Utility Functions
************/
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
    if (vkCreateImageView(LOGICAL_DEVICE, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}


/**************
GLFW Utility Functions
************/
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

		glfwSetKeyCallback(window, GLFWKeyboard);
		glfwSetCursorPosCallback(window, GLFWMouseMotion);
		glfwSetMouseButtonCallback(window, GLFWMouseButton);
}

void CApplication::createGLFWSurface() {
    if (glfwCreateWindowSurface(instance->getHandle(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}


/**************
MISC Utility Functions
************/
VkFormat CApplication::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(instance->pickedPhysicalDevice->get()->getHandle(), format, &props);

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

//static 
std::vector<char> CApplication::readFile(const std::string& filename) {
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


