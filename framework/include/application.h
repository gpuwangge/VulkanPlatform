#ifndef H_APPLICATION
#define H_APPLICATION

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "common.h"
#include "camera.hpp"
#include "instance.h"
#include "swapchain.h"
#include "dataBuffer.hpp"
#include "context.h"
#include "descriptor.h"
#include "renderProcess.h"
#include "shaderManager.h"
#include "renderer.h"
//#include "imageManager.h" 
#include "texture.h" //this includes imageManager.h
#include "modelManager.h"
#include "controller.h"

class CApplication{
public:
    CApplication();
    ~CApplication();

    GLFWwindow * window;
    int windowWidth, windowHeight;

    //VkInstance instance;//01
    std::unique_ptr<CInstance> instance{nullptr};

    VkSurfaceKHR surface;//03
    
    
    bool framebufferResized = false;

    CDebugger * debugger;

	CSwapchain swapchain;
    CRenderProcess renderProcess;
    CShaderManager shaderManager;
    CDescriptor descriptor;
    CRenderer renderer;
    CTextureImage textureImage;
    CModelManager modelManager;
    CController controller;

    //std::chrono::_V2::system_clock::time_point lastTime;

    void run();

    void prepareGLFW();
    void createGLFWSurface();
    void prepareVulkanDevices();

    void mainLoop();
    
    static Camera mainCamera; //declare static class member. But can not define and init them in the header file!
    static bool NeedToExit;


    /*Clean up Functions*/
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    
    float durationTime = 0;
    float deltaTime = 0;

    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize();
    virtual void update();
    virtual void recordCommandBuffer();
    
    /*Pure virtual function(=0): base class not implment, derived class must implement*/

};

#endif