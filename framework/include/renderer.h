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

    /**************************
     * 
     * Universial Render Functions
     * 
     * ***********************/
    typedef enum RenderModes {
        RENDER_GRAPHICS_Mode = 0,
        RENDER_COMPUTE_Mode = 1,
        RENDER_COMPUTE_SWAPCHAIN_Mode = 2,
        RENDER_COMPUTE_GRAPHICS_Mode = 3,
    } RenderModes;

    RenderModes m_renderMode = RENDER_GRAPHICS_Mode;
    //void RecordCompute();
    //void RecordGraphics();

    void AquireSwapchainImage(CSwapchain &swapchain);
    void WaitForComputeFence();
    void SubmitCompute();
    void WaitForGraphicsFence();
    void SubmitGraphics();
    void PresentSwapchainImage(CSwapchain &swapchain); 


    /**************************
     * 
     * Graphics Functions
     * 
     * ***********************/

    //void preRecordGraphicsCommandBuffer(CSwapchain &swapchain); //prepareCurrentFrameAndAcquireImageIndex
    //void postRecordGraphicsCommandBuffer(CSwapchain &swapchain);

    //Create start() and end() to make sample command recording simple
    void StartRecordGraphicsCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout, 
        VkRenderPass &renderPass, 
        std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent,
        std::vector<VkClearValue> &clearValues);
    void EndRecordGraphicsCommandBuffer();

    //Start(...)
    void BeginCommandBuffer(int commandBufferIndex);
    void BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues);
    void BindPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex);
    void SetViewport(VkExtent2D &extent);
    void SetScissor(VkExtent2D &extent);
    void BindVertexBuffer(int objectId);
    void BindIndexBuffer(int objectId);
    void BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, VkPipelineBindPoint pipelineBindPoint, uint32_t commandBufferIndex, uint32_t dynamicOffsetCount);
    void BindGraphicsDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, int dynamicOffsetCount);
    void BindComputeDescriptorSets(VkPipelineLayout &pipelineLayout,  std::vector<std::vector<VkDescriptorSet>> &descriptorSets, int dynamicOffsetCount);

    //Draw
    template <typename T>
    void PushConstantToCommand(T &pc, VkPipelineLayout graphicsPipelineLayout, VkPushConstantRange &pushConstantRange){
        vkCmdPushConstants(commandBuffers[graphicsCmdId][currentFrame], graphicsPipelineLayout, pushConstantRange.stageFlags, pushConstantRange.offset, pushConstantRange.size, &pc);
    }
    void DrawIndexed(std::vector<uint32_t> &indices3D);
    void Draw(uint32_t n);

    //End()
    void EndRenderPass();
    void EndCommandBuffer(int commandBufferIndex);


    /**************************
     * 
     * Compute Shader Functions
     * 
     * ***********************/

    //std::vector<VkCommandBuffer> commandBuffers_compute; 
    //void CreateComputeCommandBuffers();
    //void preRecordComputeCommandBuffer(CSwapchain &swapchain); //prepareCurrentFrame
    //void postRecordComputeCommandBuffer(CSwapchain &swapchain);
    //void drawComputeFrame(VkPipeline &computePipeline, VkPipelineLayout &pipelineLayout_compute, std::vector<VkDescriptorSet> &descriptorSets_compute);
    //void recordComputeCommandBuffer0(VkPipeline &computePipeline, VkPipelineLayout &pipelineLayout_compute, std::vector<VkDescriptorSet> &descriptorSets_compute);
    
    void StartRecordComputeCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout);
    void EndRecordComputeCommandBuffer();

    void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);


    /**************************
     * 
     * MISC Functions
     * 
     * ***********************/

    template <typename T>
    void CreateVertexBuffer(IN std::vector<T> &input){
        CWxjBuffer vertexDataBuffer;
        //HERE_I_AM("Init05CreateVertexBuffer");
        VkDeviceSize bufferSize = sizeof(input[0]) * input.size();

        //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexDataBuffer);//allocate vertexDataBuffer bufferSize(decided by vertices3D) memory
        VkResult result = vertexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        //REPORT("InitVertexDataBuffer");
        //FillDataBufferHelper(vertexDataBuffer, (void *)(input.data()));//copy vertices3D to vertexDataBuffer
        vertexDataBuffer.fill((void *)(input.data()));

        vertexDataBuffers.push_back(vertexDataBuffer);
    }
    void CreateIndexBuffer(std::vector<uint32_t> &indices3D);

    int graphicsCmdId;
    int computeCmdId;
    void CreateCommandPool(VkSurfaceKHR &surface);
    void CreateGraphicsCommandBuffer();
    void CreateComputeCommandBuffer();
    void CreateCommandBuffers();

    void CreateSyncObjects(int swapchainSize);

    void Destroy();

    uint32_t currentFrame;
    uint32_t imageIndex;
    void Update(); //update currentFrame

    std::vector<CWxjBuffer> vertexDataBuffers;  //each buffer object is for one model object, the index in this vector is object.id
	std::vector<CWxjBuffer> indexDataBuffers; 
    std::vector<std::vector<VkCommandBuffer>> commandBuffers;  //commandBuffers[Size][MAX_FRAMES_IN_FLIGHT or currentFrame]
    VkCommandPool commandPool;


    std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
   // std::vector<VkFence> imagesInFlight;/////////

    std::vector<VkSemaphore> computeFinishedSemaphores;
    std::vector<VkFence> computeInFlightFences;
private:
    //CDebugger * debugger;
};

#endif
