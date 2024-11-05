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

    void copyBufferToImage_cubemap(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void transitionImageLayout_cubemap(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    void CreateTextureImage(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, unsigned short bitPerTexelPerChannel = 8);
    void CreateTextureImage(void* texels, VkImageUsageFlags usage, CWxjImageBuffer &imageBuffer, unsigned short texChannels, unsigned short texBptpc);
    void CreateImageView(VkImageAspectFlags aspectFlags);

    void CreateTextureImage_cubemap(void* texels, VkImageUsageFlags usage, CWxjImageBuffer &imageBuffer, unsigned short texChannels, unsigned short texBptpc);
    void CreateImageView_cubemap(VkImageAspectFlags aspectFlags);

    void generateMipmaps(); //create normal mipmap
    void generateMipmaps(std::string rainbowCheckerboardTexturePath, VkImageUsageFlags usage); //create mix mipmaps
    void generateMipmapsCore(VkImage image, bool bCreateTempTexture = false, bool bCreateMixTexture = false, std::array<CWxjImageBuffer, MIPMAP_TEXTURE_COUNT> *textureImageBuffers_mipmaps = NULL);
    

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


class CTextureManager{
public:
    std::vector<CTextureImage> textureImages;
    //unsigned int textureImageCount = 0;
    CTextureManager();
    ~CTextureManager();
    void CreateTextureImage(
        const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, 
        bool bEnableMipmap = false,
        VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB, 
        unsigned short bitPerTexelPerChannel = 8);
    void Destroy();
};


#endif