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

class CTextureImage : public CImageManager{
public:
    CTextureImage();
    ~CTextureImage();
    void Destroy();

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool &commandPool);
    VkCommandBuffer beginSingleTimeCommands(VkCommandPool &commandPool);
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool &commandPool);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkCommandPool &commandPool);
    void CreateImage_texture(const std::string texturePath, VkImageUsageFlags usage, OUT MyImageBuffer &textureImageBuffer, OUT int32_t &texWidth, OUT int32_t &texHeight, VkCommandPool &commandPool);

    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, std::array<MyImageBuffer, MIPMAP_TEXTURE_COUNT> *textureImageBuffers_mipmaps, bool bMix, VkCommandPool &commandPool);
    void CreateRainbowMipmaps(IN OUT VkImage image, VkImageUsageFlags usage, std::string rainbowCheckerboardTexturePath, VkCommandPool &commandPool);

    //Resource for Texture
    int32_t texWidth, texHeight;
    //std::string texturePath = "";
	//VkSampler textureSampler;
	MyImageBuffer textureImageBuffer; 
	VkImageView textureImageView; 

    uint32_t mipLevels = 1; //1 means no mipmap
    bool bEnableMipMap = false;
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
