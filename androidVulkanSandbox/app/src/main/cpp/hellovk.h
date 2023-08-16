#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "samples/sample.h"

/**
 * HelloVK contains the core of Vulkan pipeline setup. It includes recording
 * draw commands as well as screen clearing during the render pass.
 *
 * Please refer to: https://vulkan-tutorial.com/ for a gentle Vulkan
 * introduction.
 */

namespace vkt {
#define LOG_TAG "hellovkjni"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define VK_CHECK(x)                           \
  do {                                        \
    VkResult err = x;                         \
    if (err) {                                \
      LOGE("Detected Vulkan error: %d", err); \
      abort();                                \
    }                                         \
  } while (0)

struct ANativeWindowDeleter {
  void operator()(ANativeWindow *window) { ANativeWindow_release(window); }
};

class HelloVK {
 public:
  void initVulkan();
  void render();
  void cleanup();
  void reset(ANativeWindow *newWindow, AAssetManager *newManager);
  bool initialized = false;

 private:
    TEST_CLASS_NAME sample;
    std::unique_ptr<ANativeWindow, ANativeWindowDeleter> window;
};

void HelloVK::initVulkan() {
    initialized = true;
    CContext::Init();
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> requiredInstanceExtensions;
    requiredInstanceExtensions.push_back("VK_KHR_surface");
    requiredInstanceExtensions.push_back("VK_KHR_android_surface");
    sample.instance = std::make_unique<CInstance>(requiredValidationLayers,requiredInstanceExtensions);

    assert(window != nullptr);  // window not initialized
    const VkAndroidSurfaceCreateInfoKHR create_info{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = window.get()};
    VK_CHECK(vkCreateAndroidSurfaceKHR(sample.instance->getHandle(), &create_info,nullptr /* pAllocator */, &sample.surface));

    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    sample.instance->findAllPhysicalDevices();

    CContext::GetHandle().physicalDevice = sample.instance->pickSuitablePhysicalDevice(sample.surface, requireDeviceExtensions, requiredQueueFamilies);

    CContext::GetHandle().physicalDevice->get()->createLogicalDevices(sample.surface, requiredValidationLayers, requireDeviceExtensions);

    //TODO: need acquire Android Screen Size
    sample.windowWidth = 960;
    sample.windowHeight = 1440;

    sample.swapchain.createSwapchainImages(sample.surface, sample.windowWidth, sample.windowHeight);
    sample.swapchain.createImageViews(VK_IMAGE_ASPECT_COLOR_BIT);

    sample.initialize();
}


void HelloVK::reset(ANativeWindow *newWindow, AAssetManager *newManager) {
    window.reset(newWindow);
    //sample.androidManager.assetManager = newManager;
    sample.modelManager.androidManager.assetManager = newManager;
    sample.textureImages.resize(3);
    sample.textureImages[0].androidManager.assetManager = newManager;
    sample.textureImages[1].androidManager.assetManager = newManager;
    sample.textureImages[2].androidManager.assetManager = newManager;
    sample.shaderManager.androidManager.assetManager = newManager;
  //if (initialized) {
  //    app.createSurface();
  //  recreateSwapChain();
  //}
}

void HelloVK::render() {
    sample.update();
    sample.renderer.prepareCurrentFrameAndAcquireImageIndex(sample.swapchain);//TODO for test compute shader
    sample.recordCommandBuffer();
    sample.renderer.drawFrame(sample.swapchain);//TODO for test compute shader
}

void HelloVK::cleanup() {
    vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());
    sample.CleanUp();
    initialized = false;
}

}  // namespace vkt