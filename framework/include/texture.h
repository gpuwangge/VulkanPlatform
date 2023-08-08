#ifndef H_TEXTURE
#define H_TEXTURE

// #include "common.h"
// #include "context.h"
// #include "dataStructure.hpp"
#include "imageManager.h"

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

#endif