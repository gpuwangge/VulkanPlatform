#ifndef H_GLFWMANAGER
#define H_GLFWMANAGER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../../vulkanFramework/include/common.h"
#include "../../vulkanFramework/include/camera.hpp"
#include "../../vulkanFramework/include/instance.h"

class CGLFWManager final{

public:
    CGLFWManager();
    ~CGLFWManager();

    GLFWwindow * window;
    int m_windowWidth, m_windowHeight;

    void createWindow(int &windowWidth, int &windowHeight, std::string windowTitle);
    void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions);
    void createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface);

    static void GLFWKeyboard(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void GLFWMouseMotion(GLFWwindow *window, double xpos, double ypos);
	static void GLFWMouseButton(GLFWwindow *window, int button, int action, int mods);
};

#endif
