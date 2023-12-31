#ifndef H_TEXTURE
#define H_TEXTURE

#include "imageBuffer.h"
#include "renderer.h"
#include "context.h"

class CTextureImage final{
public:
    CTextureImage();
    ~CTextureImage();
    void Destroy();

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    void CreateTextureImage(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, unsigned short bitPerTexelPerChannel = 8);
    void CreateTextureImage(void* texels, VkImageUsageFlags usage, CWxjImageBuffer &imageBuffer, unsigned short texChannels, unsigned short texBptpc);
    void CreateImageView(VkImageAspectFlags aspectFlags);

    void generateMipmaps();
    void generateMipmaps(VkImage image, bool bMix = false, std::array<CWxjImageBuffer, MIPMAP_TEXTURE_COUNT> *textureImageBuffers_mipmaps = NULL);
    void generateMipmaps(std::string rainbowCheckerboardTexturePath, VkImageUsageFlags usage);

    //Resource for Texture
    int32_t texWidth, texHeight;
    VkFormat imageFormat;
    short dstTexChannels;
    //std::string texturePath = "";
	//VkSampler textureSampler;
	CWxjImageBuffer textureImageBuffer;
	//VkImageView textureImageView;

    VkCommandPool *pCommandPool;

    uint32_t mipLevels = 1; //1 means no mipmap
    bool bEnableMipMap = false;
};

#endif