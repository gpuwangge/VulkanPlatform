#include "application.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

CApplication::CApplication(){
    debugger = new CDebugger("../logs/application.log");

    mainCamera.type = Camera::CameraType::firstperson;
    mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    mainCamera.movementSpeed = 1.0f;
    mainCamera.rotationSpeed = 200.0f;
}

void CApplication::run(){
    prepareGLFW();
    prepareVulkanDevices();
    initialize();
    mainLoop();
}

void CApplication::initialize(){
    renderer.CreateSyncObjects();
    shaderManager.Destroy();
}

void CApplication::prepareVulkanDevices(){
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    
    instance = std::make_unique<CInstance>(requiredValidationLayers);

    //need instance. Surface is to store view format information for creating swapchain. 
    //Only GLFW knows what kind of surface can be attached to its window.
    createGLFWSurface();

    instance->findAllPhysicalDevices();

    CContext::Init();
    CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);

    //App dev can only query properties from physical device, but can not directly operate it
    //App dev operates logical device, can logical device communicate with physical device by command queues
    //App dev will fill command buffer with commands later
    //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
}

void CApplication::recordCommandBuffer(){

}

void CApplication::update(){
    //printf("app update...\n");

    static auto startTime = std::chrono::high_resolution_clock::now();
    static auto lastTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    durationTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = currentTime;

    mainCamera.update(deltaTime);

    descriptor.updateMVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera);

    renderer.Update();
}

void CApplication::mainLoop(){
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
            update();
            renderer.prepareCurrentFrameAndAcquireImageIndex(swapchain);
            recordCommandBuffer();
            renderer.drawFrame(swapchain);
			if (NeedToExit) break;
		}

		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
}

void CApplication::wxjCreateFramebuffers(){
	HERE_I_AM("wxjCreateFramebuffers");

	VkResult result = VK_SUCCESS;

	swapchain.swapChainFramebuffers.resize(swapchain.imageManager.views.size());

	for (size_t i = 0; i < swapchain.imageManager.imageSize; i++) {
		std::vector<VkImageView> attachments; 
		 if (renderProcess.bUseDepthAttachment && renderProcess.bUseColorAttachmentResolve) {//Renderpass attachment(render target) order: Color, Depth, ColorResolve
		    attachments.push_back(swapchain.imageManager.msaaColorImageView);
		    attachments.push_back(swapchain.imageManager.depthImageView);
			attachments.push_back(swapchain.imageManager.views[i]);
		 }else if(renderProcess.bUseDepthAttachment){//Renderpass attachment(render target) order: Color, Depth
			attachments.push_back(swapchain.imageManager.views[i]);
			attachments.push_back(swapchain.imageManager.depthImageView);
		}else{ //Renderpass attachment(render target) order: Color
			attachments.push_back(swapchain.imageManager.views[i]);
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderProcess.renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();//swapChainImageViews, 类型是VkImageView
		framebufferInfo.width = swapchain.swapChainExtent.width;
		framebufferInfo.height = swapchain.swapChainExtent.height;
		framebufferInfo.layers = 1;

		result = vkCreateFramebuffer(CContext::GetHandle().GetLogicalDevice(), &framebufferInfo, nullptr, &swapchain.swapChainFramebuffers[i]);
		if (result != VK_SUCCESS) throw std::runtime_error("failed to create framebuffer!");
		REPORT("vkCreateFrameBuffer");
	}	
}



// void CVulkanBase::wxjCreateImageView(IN VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, int mipLevel, OUT VkImageView &imageView){
//     imageView = createImageView(image, format, aspectFlags, mipLevel);
// }
// void CVulkanBase::wxjCreateImage(VkSampleCountFlagBits numSamples, VkFormat format, VkImageUsageFlags usage, OUT MyImageBuffer &imageBuffer){
// 	//do not use mipLevels here, because only depth and msaa(colorResolve) buffer calls this, they dont need mipmap
// 	//texture (w/o mipmap) has another function to create image
// 	createImage(swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, 1, numSamples, format, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageBuffer);
// }

void CApplication::wxjCreateSwapChainImagesAndImageViews(){
	HERE_I_AM("Init08Swapchain");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    swapchain.createImages(surface, width, height);

    //generate swapChainImageViews
    //for (size_t i = 0; i < swapchain.imageManager.imageSize; i++) {
        //swapchain.swapchainImage.swapChainImageViews[i] = createImageView(swapchain.swapChainImages[i], swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	//	swapchain.imageManager.views[i] = swapchain.imageManager.createImageView(swapchain.imageManager.images[i], swapchain.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        //REPORT("vkCreateImageView");
    //}
    swapchain.createImageViews(VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void CApplication::wxjLoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelName.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex3D, uint32_t> uniqueVertices{};

	//vertices3D.clear();
	//indices3D.clear();

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex3D vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices3D.size());
				vertices3D.push_back(vertex);
			}

			indices3D.push_back(uniqueVertices[vertex]);
		}
	}
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

    swapchain.CleanUp();
    renderProcess.Cleanup();
    descriptor.DestroyAndFree();
    textureImage.Destroy();
    renderer.Destroy();

    vkDestroyDevice(CContext::GetHandle().GetLogicalDevice(), nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance->getHandle(), instance->debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    vkDestroyInstance(instance->getHandle(), nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    delete debugger;

    CContext::Quit();
}

