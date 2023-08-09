#ifndef H_RENDERER
#define H_RENDERER

#include "common.h"
#include "context.h"
#include "dataBuffer.hpp"
#include "swapchain.h"

class CRenderer final{
public:
    CRenderer();
    ~CRenderer();

    template <typename T>
    void CreateVertexBuffer(IN std::vector<T> &input){
        HERE_I_AM("Init05CreateVertexBuffer");
        VkDeviceSize bufferSize = sizeof(input[0]) * input.size();

        //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexDataBuffer);//allocate vertexDataBuffer bufferSize(decided by vertices3D) memory
        VkResult result = vertexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        REPORT("InitVertexDataBuffer");
        //FillDataBufferHelper(vertexDataBuffer, (void *)(input.data()));//copy vertices3D to vertexDataBuffer
        vertexDataBuffer.fill((void *)(input.data()));
    }

    void CreateIndexBuffer(std::vector<uint32_t> &indices3D);
    void InitCreateCommandPool(VkSurfaceKHR &surface);
    void InitCreateCommandBuffers();
    void CreateSyncObjects();

    void BeginCommandBuffer();
    void BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues);
    void BindPipeline(VkPipeline &pipeline);
    void SetViewport(VkExtent2D &extent);
    void SetScissor(VkExtent2D &extent);
    void BindVertexBuffer();
    void BindIndexBuffer();
    void BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<VkDescriptorSet> &descriptorSets);
    void DrawIndexed(std::vector<uint32_t> &indices3D);
    void Draw(uint32_t n);
    void EndRenderPass();
    void EndCOmmandBuffer();

    void prepareCurrentFrameAndAcquireImageIndex(CSwapchain &swapchain);
    void drawFrame(CSwapchain &swapchain);

    void Destroy();

    uint32_t currentFrame;
    uint32_t imageIndex;
    void Update();

    CWxjBuffer vertexDataBuffer;  //05
	CWxjBuffer indexDataBuffer; //05
    std::vector<VkCommandBuffer> commandBuffers;//06
    VkCommandPool commandPool;//06

    std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

private:
    CDebugger * debugger;
};

#endif
