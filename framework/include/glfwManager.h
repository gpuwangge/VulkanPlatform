#ifndef H_GLFWMANAGER
#define H_GLFWMANAGER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "common.h"
#include "camera.hpp"
#include "instance.h"

class CGLFWManager final{

public:
    CGLFWManager();
    ~CGLFWManager();

    GLFWwindow * window;
    int windowWidth, windowHeight;

    void prepareGLFW();
    void createGLFWSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface);
    void getGLFWRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions);

    static void GLFWKeyboard(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void GLFWMouseMotion(GLFWwindow *window, double xpos, double ypos);
	static void GLFWMouseButton(GLFWwindow *window, int button, int action, int mods);
};

#endif
