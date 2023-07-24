#ifndef H_VULKANBASE
#define H_VULKANBASE

#include "application.h"

class CVulkanBase : public CApplication{
public:
    CVulkanBase();
    ~CVulkanBase();

    
    /**************
    Interface function to vulkan samples
    ************/
    //void wxjSetVertices3D(float input[]);

    /**************
    Application operation functions
    ************/
    void updateUniformBuffer(uint32_t currentFrame, float durationTime);
   	void update();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void drawFrame();
    void initVulkan();
};

#endif