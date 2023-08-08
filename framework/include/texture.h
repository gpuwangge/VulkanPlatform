#ifndef H_TEXTURE
#define H_TEXTURE

// #include "common.h"
// #include "context.h"
// #include "dataStructure.hpp"
#include "imageManager.h"
#include "renderer.h"

class CTextureImage : private CImageManager{
public:
    CTextureImage();
    ~CTextureImage();
    void Destroy();

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CreateImage(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool);
    void CreateImage(const std::string texturePath, VkImageUsageFlags usage, MyImageBuffer &imageBuffer, VkCommandPool &commandPool);
    void CreateImageView(VkImageAspectFlags aspectFlags);

    void generateMipmaps();
    void generateMipmaps(VkImage image, bool bMix = false, std::array<MyImageBuffer, MIPMAP_TEXTURE_COUNT> *textureImageBuffers_mipmaps = NULL);
    void generateMipmaps(std::string rainbowCheckerboardTexturePath, VkImageUsageFlags usage);

    //Resource for Texture
    int32_t texWidth, texHeight;
    VkFormat imageFormat;
    //std::string texturePath = "";
	//VkSampler textureSampler;
	MyImageBuffer textureImageBuffer;
	VkImageView textureImageView;

    VkCommandPool *pCommandPool;

    uint32_t mipLevels = 1; //1 means no mipmap
    bool bEnableMipMap = false;
};

#endif