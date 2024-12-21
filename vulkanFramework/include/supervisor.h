#ifndef H_SUPERVISOR
#define H_SUPERVISOR
#include "dataBuffer.hpp"
#include "modelManager.h"

/***************
There is no such thing as a static class in C++. 
The closest approximation is a class that only contains static data members and static methods.
Static data members in a class are shared by all the class objects as there is only one copy of them in the memory, 
regardless of the number of objects of the class. Static methods in a class can only access static data members, 
other static methods or any methods outside the class.
*****************/

//forward declaration. 
//Because we dont want to include application.h here, but we want to use CApplciation.
//We want application to include supervisor.h instead
class CApplication;

class CSupervisor {
public:
    CSupervisor(){}
    static CApplication *m_app;
    static void Register(CApplication *app);//need command pool for texture creation
    /*****************************
     * 
     * Shaders
     * 
     *****************************/
    //static std::string VertexShader;
    //static std::string FragmentShader;
    //static std::string ComputeShader;
    static bool Query_Pipeline_Graphics();
    static bool Query_Pipeline_Compute();

    /*****************************
     * 
     * Graphics Vectors
     * 
     *****************************/
    static std::vector<Vertex2D> vertices2D;
    static std::vector<Vertex3D> vertices3D;
    static std::vector<uint32_t> indices3D;

    /*****************************
     * 
     * Graphcis Uniform
     * 
     *****************************/
    static VkDeviceSize GraphicsCustomUniformBufferSize;
    static VkDescriptorSetLayoutBinding GraphicsCustomBinding; 
    static int m_samplerCount;

    static void Activate_Uniform_Graphics_VP();
    static bool Query_Uniform_Graphics_VP();

    static void Activate_Uniform_Graphics_MVP();
    static bool Query_Uniform_Graphics_MVP();

    static void Activate_Uniform_Graphics_Custom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding);
    static bool Query_Uniform_Graphics_Custom();
    
    static void Activate_Uniform_Graphics_Sampler(int samplerCount = 1);
    static bool Query_Uniform_Graphics_Sampler();

    /*****************************
     * 
     * Compute Uniform
     * 
     *****************************/
    static VkDeviceSize ComputeCustomUniformBufferSize;
    static VkDescriptorSetLayoutBinding ComputeCustomBinding;     
    static VkDeviceSize ComputeStorageBufferSize;
    static VkBufferUsageFlags ComputeStorageBufferUsage;

    static void Activate_Uniform_Compute_Custom(VkDeviceSize graphicsCustomUniformBufferSize, VkDescriptorSetLayoutBinding graphicsCustomBinding);
    static bool Query_Uniform_Compute_Custom();

    static void Activate_Uniform_Compute_StorageBuffer(VkDeviceSize computeStorageBufferSize, VkBufferUsageFlags storageBufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    static bool Query_Uniform_Compute_StorageBuffer();

    static void Activate_Uniform_Compute_StorageImage();  //as input
    static bool Query_Uniform_Compute_StorageImage();

    static void Activate_Uniform_Compute_StorageImage_Swapchain(); //as output
    static bool Query_Uniform_Compute_StorageImage_Swapchain();

    /*****************************
     * 
     * Graphcis Feature
     * 
     *****************************/
    static bool b48bpt;
    static bool bPushConstant;
    static bool bBlend;
    static bool bRainbowMipmap;
    static void Activate_Feature_Graphics_DepthTest();
    static void Activate_Feature_Graphics_MSAA();
    static void Activate_Feature_Graphics_48BPT();
    static void Activate_Feature_Graphics_PushConstant();
    static void Activate_Feature_Graphics_Blend();
    static void Activate_Feature_Graphics_RainbowMipmap();

    /*****************************
     * 
     * Graphcis Buffer
     * 
     *****************************/    
    static VertexStructureTypes VertexStructureType;
    static void Activate_Buffer_Graphics_Vertex(std::unique_ptr<std::vector<std::string>> modelNames, CModelManager &modelManager);
    static void Activate_Buffer_Graphics_Vertex(VertexStructureTypes vertexStructureType);

    /*****************************
     * 
     * Texture
     * 
     *****************************/
    static void Activate_Texture(std::unique_ptr<std::vector<TextureAttributeInfo>> textureAttributes = NULL, bool bCubemap = false);

    /*****************************
     * 
     * Pipeline
     * 
     *****************************/
    static void Activate_Pipeline(); //*customBinding = NULL

    /*****************************
     * 
     * Compute Dispatch
     * 
     *****************************/
    static void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);
};

#endif