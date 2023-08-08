#ifndef H_APPLICATION
#define H_APPLICATION

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "common.h"
#include "camera.hpp"
#include "instance.h"
#include "swapchain.h"
#include "dataStructure.hpp"
#include "context.h"
#include "descriptor.h"
#include "renderProcess.h"
#include "shaderManager.h"
#include "renderer.h"
//#include "imageManager.h"
#include "texture.h"

class CApplication{
public:
    CApplication();
    ~CApplication();

    GLFWwindow * window;

    //VkInstance instance;//01
    std::unique_ptr<CInstance> instance{nullptr};

    VkSurfaceKHR surface;//03
    
    bool framebufferResized = false;

    CDebugger * debugger;

	CSwapchain swapchain;
    CRenderProcess renderProcess;
    CShaderManager shaderManager;
    CDescriptor descriptor;

    //std::chrono::_V2::system_clock::time_point lastTime;

    void run();

    void prepareGLFW();
    void createGLFWSurface();
    void prepareVulkanDevices();

    void mainLoop();
    
    static Camera mainCamera;
    static bool NeedToExit;
    static void GLFWKeyboard(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void GLFWMouseMotion(GLFWwindow *window, double xpos, double ypos);
	static void GLFWMouseButton(GLFWwindow *window, int button, int action, int mods);


    /*Clean up Functions*/
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    
    float durationTime = 0;
    float deltaTime = 0;

    CRenderer renderer;
    CTextureImage textureImage;
    
    void wxjLoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);

    void wxjCreateSwapChainImagesAndImageViews();

    void wxjCreateFramebuffers();

    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize();
    virtual void update();
    virtual void recordCommandBuffer();
    
    /*Pure virtual function(=0): base class not implment, derived class must implement*/

};

#endif