#ifndef H_DESCRIPTOR
#define H_DESCRIPTOR

#include "context.h"
#include "dataBuffer.hpp"
#include "camera.hpp"
//#include "object.h"
#include "../include/texture.h"

class CObject;

typedef enum UniformBufferBits {
    UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT = 0x00000001, //assume app uses one: graphics custom or compute custom
    UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT = 0x00000002,
    UNIFORM_BUFFER_MVP_BIT = 0x00000004, //assume app uses one: MVP or VP
    UNIFORM_BUFFER_VP_BIT = 0x00000008,
    UNIFORM_BUFFER_SAMPLER_BIT = 0x00000010,
    UNIFORM_BUFFER_STORAGE_BIT = 0x00000020,
    UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT = 0x00000040,
    UNIFORM_IMAGE_STORAGE_TEXTURE_BIT = 0x00000080,
    UNIFORM_BUFFERG_BITS_MAX_ENUM = 0x7FFFFFFF
} UniformBufferBits;


//this class to create descript pool. 
// custom uniform will be implemented by inherited class(graphics and compute), because graphics and compute can use custom buffer at the same time
class CDescriptorManager{
public:
    CDescriptorManager(){}
    ~CDescriptorManager(){}

    static unsigned int uniformBufferUsageFlags;
    static unsigned int samplerCount; 
    static VkDescriptorPool descriptorPool;
    static std::vector<VkDescriptorPoolSize> poolSizes;

    static unsigned int objectCount;

    static void createDescriptorPool(unsigned int object_count = 0);//VkDescriptorType    type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    static int getPoolSize();

};

//this class to create graphics descript layer(static), and non-texture descripter set(static), create sampler(static)
//texture descripter leave to CObject
//also provide function to update non-texture uniform(static) (TODO)
class CGraphicsDescriptorManager: public CDescriptorManager{
public:
    CGraphicsDescriptorManager();
    ~CGraphicsDescriptorManager();
    
    static std::vector<VkDescriptorSetLayoutBinding> graphicsBindings;

    //Custom Uniform Buffer, same code for graphics/compute(TODO: can improve this part later)
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    static void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    template <typename T>
    void updateCustomUniformBuffer(uint32_t currentFrame, float durationTime, T customUniformBufferObject){
        if(uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_GRAPHICS_BIT)
            memcpy(customUniformBuffersMapped[currentFrame], &customUniformBufferObject, sizeof(customUniformBufferObject));
    }

    //bool bUseMVP;
    static std::vector<CWxjBuffer> mvpUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
	static std::vector<void*> mvpUniformBuffersMapped;
    static void addMVPUniformBuffer();
    static MVPUniformBufferObject mvpUBO;
    void updateMVPUniformBuffer(uint32_t currentFrame, float durationTime, Camera &mainCamera, std::vector<CObject> &objectList);
    //bool bUseVP;
    static std::vector<CWxjBuffer> vpUniformBuffers; 
	static std::vector<void*> vpUniformBuffersMapped;
    static void addVPUniformBuffer();
    static VPUniformBufferObject vpUBO;
    void updateVPUniformBuffer(uint32_t currentFrame, float durationTime, Camera &mainCamera);
    static bool CheckMVP(); //to check if all objects associate this graphcis descriptor use MVP/VP or not. If return true, means it will use dynamic descriptor offset

    //bool bUseSampler;
    //static int textureSamplerCount;
    static std::vector<VkSampler> textureSamplers;
    static void addImageSamplerUniformBuffer(uint32_t mipLevels);

    static int getLayoutSize();
    static int getSetSize();
    static void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);
    void createDescriptorSets(std::vector<CTextureImage> *textureImages = NULL, std::vector<VkImageView> *swapchainImageViews = NULL);

        //VkDescriptorType      descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        //VkShaderStageFlags    stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        //uint32_t              descriptorCount = 1,
        //const VkSampler*      pImmutableSamplers = nullptr);
    //static void createMVPDescriptorSetLayout();
    static void createTextureDescriptorSetLayout();

    void DestroyAndFree();

    
    static VkDescriptorSetLayout descriptorSetLayout;
    //static VkDescriptorSetLayout mvpDescriptorSetLayout;
    static VkDescriptorSetLayout textureDescriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    //CDebugger * debugger;
};

//this class to create compute descript layer(static), and descript set(static)
class CComputeDescriptorManager: public CDescriptorManager{
public:
    CComputeDescriptorManager(){
        std::cout<<"CComputeDescriptorManager construct."<<std::endl;
    }

    ~CComputeDescriptorManager(){

    }

    static VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)

    static std::vector<VkDescriptorSetLayoutBinding> computeBindings;

    //Custom Uniform Buffer, same code for graphics/compute(TODO: can improve this part later)
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    static void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    template <typename T>
    void updateCustomUniformBuffer(uint32_t currentFrame, float durationTime, T customUniformBufferObject){
        if(uniformBufferUsageFlags & UNIFORM_BUFFER_CUSTOM_COMPUTE_BIT)
            memcpy(customUniformBuffersMapped[currentFrame], &customUniformBufferObject, sizeof(customUniformBufferObject));
    }

   //bool bUseStorageBuffer;
    //define two set of storage buffers, one for input, the other for output
	static std::vector<CWxjBuffer> storageBuffers; 
    static std::vector<void*> storageBuffersMapped;
    static VkDeviceSize m_storageBufferSize;
    //std::vector<CWxjBuffer> storageBuffers_2; 
    //std::vector<void*> storageBuffersMapped_2;
    //VkDeviceSize m_storageBufferSize_2;
    static void addStorageBuffer(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT); //the same function to add storage 1&2
    template <typename T>
    void updateStorageBuffer(uint32_t currentFrame, float durationTime, T storageBufferObject){ 
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_1_BIT)
        memcpy(storageBuffersMapped[currentFrame], &storageBufferObject, sizeof(storageBufferObject));
    }
    //template <typename T>
    //void updateStorageBuffer_2(uint32_t currentFrame, float durationTime, T storageBufferObject){ 
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT)
    //    memcpy(storageBuffersMapped_2[currentFrame], &storageBufferObject, sizeof(storageBufferObject));
    //}

    //for storage image
    //std::vector<CWxjBuffer> storageImages; 
    //std::vector<void*> storageImagesMapped;
    //VkDeviceSize m_storageImageSize;
    static void addStorageImage(VkBufferUsageFlags usage);
    // void updateStorageImage();    


    static int getLayoutSize();
    static int getSetSize();
    static void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);
    void createDescriptorSets(std::vector<CTextureImage> *textureImages = NULL, std::vector<VkImageView> *swapchainImageViews = NULL);


    void DestroyAndFree();
};



#endif