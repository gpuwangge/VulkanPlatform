#include "application.h"

Camera CApplication::mainCamera;  //define static class members
bool CApplication::NeedToExit = false; 

CApplication::CApplication(){
    debugger = new CDebugger("../logs/application.log");

    mainCamera.type = Camera::CameraType::firstperson;
    mainCamera.setPosition(glm::vec3(0.0f, -2.5f, -2.5f));
    mainCamera.setRotation(glm::vec3(45.0f, 0.0f, 0.0f));
    mainCamera.setPerspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 256.0f);
    mainCamera.movementSpeed = 1.0f;
    mainCamera.rotationSpeed = 200.0f;

    windowWidth = 0;
    windowHeight = 0;

    //NeedToExit = false;
}

void CApplication::run(){
    prepareGLFW();
    prepareVulkanDevices();
    initialize();
    mainLoop();
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

    swapchain.createSwapchainImages(surface, windowWidth, windowHeight);
	swapchain.createImageViews(VK_IMAGE_ASPECT_COLOR_BIT);
}

void CApplication::initialize(){
    renderer.CreateSyncObjects();
    shaderManager.Destroy();
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
    descriptor.updateVPUniformBuffer(renderer.currentFrame, durationTime, mainCamera);

    renderer.Update();
}

void CApplication::mainLoop(){
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
            update();
            if(!descriptor.bUseComputeStorage) renderer.prepareCurrentFrameAndAcquireImageIndex(swapchain);//TODO for test compute shader 
            recordCommandBuffer();
            if(!descriptor.bUseComputeStorage) renderer.drawFrame(swapchain);//TODO for test compute shader
			if (NeedToExit) break;
		}

		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
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

		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		glfwSetKeyCallback(window, controller.GLFWKeyboard);
		glfwSetCursorPosCallback(window, controller.GLFWMouseMotion);
		glfwSetMouseButtonCallback(window, controller.GLFWMouseButton);
}

void CApplication::createGLFWSurface() {
    if (glfwCreateWindowSurface(instance->getHandle(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
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

    if (enableValidationLayers) 
        DestroyDebugUtilsMessengerEXT(instance->getHandle(), instance->debugMessenger, nullptr);

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    vkDestroyInstance(instance->getHandle(), nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    delete debugger;

    CContext::Quit();
}

