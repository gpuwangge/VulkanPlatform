#ifndef H_SDLMANAGER
#define H_SDLMANAGER

// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif


#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "common.h"
#include "camera.hpp"
#include "instance.h"

class CSDLManager final{

public:
    CSDLManager();
    ~CSDLManager();

    //GLFWwindow * window;
    SDL_Window* window;
    int m_windowWidth, m_windowHeight;

    bool bStillRunning = true;
    bool bKeyDown = false;
    bool bMouseDown = false;

    void createWindow(int &windowWidth, int &windowHeight);
    void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions);
    void createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface);

    void eventHandle();
};

#endif
