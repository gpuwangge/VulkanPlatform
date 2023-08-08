#ifndef H_IMAGEMANAGER
#define H_IMAGEMANAGER

#include "common.h"
#include "context.h"
#include "dataStructure.hpp"

class CImageManager{

public:
    CImageManager();
    ~CImageManager();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) ;
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, MyImageBuffer &imageBuffer);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    CDebugger * debugger;
};



class CSwapchainImage : public CImageManager{
public:
    CSwapchainImage();
    ~CSwapchainImage();
    void Destroy();

    void GetMaxUsableSampleCount();

    std::vector<VkImage> swapChainImages;//08
    std::vector<VkImageView> swapChainImageViews;//08

    //Resource for Depth Test
    bool bEnableDepthTest = false;
	MyImageBuffer depthImageBuffer;
	VkImageView depthImageView;

    //Resource for MSAA
    bool bEnableMSAA;
    MyImageBuffer msaaColorImageBuffer;
	VkImageView msaaColorImageView;
    VkSampleCountFlagBits msaaSamples;
};

#endif
