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

// #define LOGICAL_DEVICE *(instance->pickedPhysicalDevice->get()->getLogicalDevice())
// #define PHYSICAL_DEVICE instance->pickedPhysicalDevice->get()->getHandle()
// #define GRAPHICS_QUEUE *(instance->pickedPhysicalDevice->get()->getGraphicsQueue()) 
// #define PRESENT_QUEUE *(instance->pickedPhysicalDevice->get()->getPresentQueue()) 
// #define COMPUTE_QUEUE *(instance->pickedPhysicalDevice->get()->getComputeQueue()) 

class CApplication{
public:
    CApplication();
    ~CApplication();

    GLFWwindow * window;

    //VkInstance instance;//01
    std::unique_ptr<CInstance> instance{nullptr};

    VkSurfaceKHR surface;//03
    
    //std::vector<Vertex3D> vertices3D;
    //std::vector<uint32_t> indices3D;
    bool framebufferResized = false;

    CDebugger * debugger;

	CWxjBuffer vertexDataBuffer;  //05
	CWxjBuffer indexDataBuffer; //05
    //std::vector<CWxjBuffer> uniformBuffers; //05 MVP uniform
	//std::vector<void*> uniformBuffersMapped; //05 MVP uniform

    std::vector<VkCommandBuffer> commandBuffers;//06
    VkCommandPool commandPool;//06

	CSwapchain swapchain;

    //VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    //VkFormat findDepthFormat();
    //VkRenderPass renderPass; //10
    CRenderProcess renderProcess;//new

    //std::vector<VkFramebuffer> swapChainFramebuffers;//11

    VkShaderModule vertShaderModule;//12
	VkShaderModule fragShaderModule;//12

    // VkDescriptorPool descriptorPool;
    // VkDescriptorSetLayout descriptorSetLayout;
    // std::vector<VkDescriptorSet> descriptorSets;
    CDescriptor descriptor;

    //VkPipelineLayout pipelineLayout;
	//VkPipeline graphicsPipeline;

    std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t currentFrame = 1;
    std::chrono::_V2::system_clock::time_point lastTime;

    //Resource for Texture
    int32_t texWidth, texHeight;
    //std::string texturePath = "";
	//VkSampler textureSampler;
	MyImageBuffer textureImageBuffer; 
	VkImageView textureImageView; 

    //Resource for Depth Test
    //bool bEnableDepthTest;
	MyImageBuffer depthImageBuffer;
	VkImageView depthImageView;

    //Resource for MSAA
    bool bEnableMSAA;
    MyImageBuffer msaaColorImageBuffer;
	VkImageView msaaColorImageView;
    VkSampleCountFlagBits msaaSamples;

    void run();

    void prepareGLFW();
    void createGLFWSurface();
    void prepareVulkanDevices();

    void Init06CreateCommandPool();
    void Init06CreateCommandBuffers();

    // VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    // VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    // VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);


    std::vector<char> readFile(const std::string& filename);
    void Init12SpirvShader(std::string filename, VkShaderModule * pShaderModule);

    void createSyncObjects();

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
    // MVPUniformBufferObject mvpUBO{};
    // void updateUniformBuffer(uint32_t currentFrame, float durationTime);

    uint32_t imageIndex = 0;
    //void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();

    //texture utility functions
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, MyImageBuffer &imageBuffer);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    
    uint32_t mipLevels = 1; //1 means no mipmap
	bool bEnableMipMap = false;
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, std::array<MyImageBuffer, MIPMAP_TEXTURE_COUNT> *tmpTextureBufferForRainbowMipmaps, bool bEnableRainbowMipmaps);
 
    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize();
    virtual void update();
    virtual void recordCommandBuffer();
    
    /*Pure virtual function(=0): base class not implment, derived class must implement*/

};

#endif