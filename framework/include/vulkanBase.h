#ifndef H_VULKANBASE
#define H_VULKANBASE

#include "application.h"

class CVulkanBase : public CApplication{
public:
    CVulkanBase(){}
    ~CVulkanBase(){}

    /**************
    Interface function to vulkan samples
    ************/
    void wxjLoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);

    template <typename T>
    void wxjCreateVertexBuffer(IN std::vector<T> &input){
        HERE_I_AM("Init05CreateVertexBuffer");
        VkDeviceSize bufferSize = sizeof(input[0]) * input.size();

        //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexDataBuffer);//allocate vertexDataBuffer bufferSize(decided by vertices3D) memory
        VkResult result = vertexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        REPORT("InitVertexDataBuffer");
        //FillDataBufferHelper(vertexDataBuffer, (void *)(input.data()));//copy vertices3D to vertexDataBuffer
        vertexDataBuffer.fill((void *)(input.data()));
    }

    void wxjCreateIndexBuffer(std::vector<uint32_t> &indices3D);

    void wxjCreateCommandBuffer();

    void wxjCreateSwapChainImagesAndImageViews();

    void wxjCreateVertexShader(std::string shaderName);
    void wxjCreateFragmentShader(std::string shaderName);

    void wxjCreateFramebuffers();

    void wxjCreateImage_texture(const std::string texturePath, VkImageUsageFlags usage, OUT MyImageBuffer &textureImageBuffer, OUT int32_t &texWidth, OUT int32_t &texHeight);
    void wxjCreateImageView(IN VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, int mipLevel, OUT VkImageView &imageView);
    void wxjCreateImage(VkSampleCountFlagBits numSamples, VkFormat format, VkImageUsageFlags usage, OUT MyImageBuffer &imageBuffer);

    void wxjBeginCommandBuffer();
    void wxjBeginRenderPass(std::vector<VkClearValue> &clearValues);
    void wxjBindPipeline();
    void wxjSetViewport();
    void wxjSetScissor();
    void wxjBindVertexBuffer();
    void wxjBindIndexBuffer();
    void wxjBindDescriptorSets();
    void wxjDrawIndexed(std::vector<uint32_t> &indices3D);
    void wxjDraw(uint32_t n);
    void wxjEndRenderPass();
    void wxjEndCOmmandBuffer();

    void wxjGetMaxUsableSampleCount();
    void wxjCreateMipmaps(IN OUT VkImage image);
    void wxjCreateMipmaps(IN OUT VkImage image, VkImageUsageFlags usage, std::string rainbowCheckerboardTexturePath);
    /**************
    Application operation functions
    ************/
    // void initialize();
    // void update();
    // void recordCommandBuffer();
};

#endif