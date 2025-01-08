#ifndef H_APPLICATION
#define H_APPLICATION

#include "common.h"
#include "camera.hpp"
#include "instance.h"
#include "swapchain.h"
#include "dataBuffer.hpp"
#include "context.h"
#include "descriptor.h"
#include "renderProcess.h"
#include "shaderManager.h"
#include "renderer.h"
#include "texture.h" //this includes imageManager.h
#include "modelManager.h"

#include "supervisor.h"
#include "object.h"

//Macro to convert the macro value to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#if WIN32
    #define YAML_CPP_STATIC_DEFINE
#endif
#include "../../thirdParty/yaml-cpp/yaml.h"

#ifndef ANDROID
    #ifndef SDL
        #include "..\\..\\glfwFramework\\include\\glfwManager.h"
    #else
        #include "..\\..\\sdlFramework\\include\\sdlManager.h"
    #endif
#endif

//Decide not to use macro
// #define START_GRAPHICS_RECORD(descriptorIdx) { \
//     renderer.StartRecordGraphicsCommandBuffer(renderProcess.graphicsPipeline, renderProcess.graphicsPipelineLayout, \
//     renderProcess.renderPass,   \
//     swapchain.swapChainFramebuffers,swapchain.swapChainExtent,   \
//     descriptors[descriptorIdx].descriptorSets,  \
//     clearValues);}
// #define END_GRAPHICS_RECORD renderer.EndRecordGraphicsCommandBuffer();

//Decide not to use macro
//#define START_COMPUTE_RECORD(descriptorIdx) renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout, descriptors[descriptorIdx].descriptorSets);
//#define END_COMPUTE_RECORD renderer.EndRecordComputeCommandBuffer();

class CApplication{
public:
    CApplication();
    ~CApplication();

    //VkInstance instance;//01
    std::unique_ptr<CInstance> instance{nullptr};

    int windowWidth, windowHeight;
    VkSurfaceKHR surface;//03
    
    //bool framebufferResized = false;
    //bool needWindow = false;

	CSwapchain swapchain;
    CRenderProcess renderProcess;
    CShaderManager shaderManager;
    CGraphicsDescriptorManager graphicsDescriptorManager;
    CComputeDescriptorManager computeDescriptorManager;
    CRenderer renderer;
    CModelManager modelManager;
    CTextureManager textureManager;

    static std::vector<CObject> objectList;

    void CleanUp();

    void UpdateRecordRender();
    
#ifndef ANDROID
    #ifndef SDL
        CGLFWManager glfwManager;
    #else
        CSDLManager sdlManager;
    #endif
    void run();
#endif

    //for static class member. But can not define and init them in the header file!
    static Camera mainCamera; 
    static bool NeedToExit;
    static bool NeedToPause;
    static int LightCount;

    /*Clean up Functions*/
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    
    float durationTime = 0;
    float deltaTime = 0;

    std::string m_sampleName;

    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize(); //use this to call sample initialization
    //void initialize(); //base: create sync object, destroy shader resource
    virtual void update(); //base: update time, frame id, camera and ubo
    virtual void recordGraphicsCommandBuffer();
    virtual void recordComputeCommandBuffer();
    virtual void postUpdate();

    /*Pure virtual function(=0): base class not implment, derived class must implement*/
    //NA

    /*************
     * Helper Functions
     *******/
    void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);  


    /*************
     * APP INFO
     *******/
    struct RenderInfo{
        CRenderer::RenderModes Mode = CRenderer::RENDER_GRAPHICS_Mode;
    };
    struct UniformInfo{
        std::vector<std::vector<bool>> GraphicsVector;
        std::vector<std::vector<bool>> ComputeVector;
        std::vector<int> SamplerMiplevels;

        struct GraphicsCustomInfo{
            //bool Enable = false;
            VkDeviceSize Size = 0;
            VkDescriptorSetLayoutBinding Binding;
            //CustomUniformBufferObject* pointer; 
        }GraphicsCustom;
        //--------------------------------------
        struct ComputeCustomInfo{
            VkDeviceSize Size = 0;
            VkDescriptorSetLayoutBinding Binding;
        }ComputeCustom;
        struct ComputeStorageBufferInfo{
            VkDeviceSize Size = 0;
            VkBufferUsageFlags Usage;
        }ComputeStorageBuffer;
        //bool EnableComputeStorageImage = false; //yaml
        //bool EnableComputeStorageImageSwapChain = false; //yaml
    };
    struct FeatureInfo{
        bool EnableGraphicsDepthTest = false; //yaml
        bool EnableGraphicsMSAA = false; //yaml
        bool EnableGraphics48BPT = false; //yaml
        bool EnableGraphicsPushConstant = false; //yaml
        bool EnableGraphicsBlend = false; //yaml
        bool EnableGraphicsRainbowMipmap = false; //yaml
        //bool EnableGraphicsCubemap = false;
        int GraphicsPipelineSkyboxID = -1; //yaml
    };
    struct BufferInfo{
        struct GraphicsVertexInfo{
            //bool Enable = false;
            VertexStructureTypes StructureType = (VertexStructureTypes)NULL;
            //std::vector<Vertex3D> *Vertices3D = NULL;
            //std::vector<uint32_t> *Indices3D = NULL;
            std::vector<Vertex2D> *Vertices2D = NULL;
        }GraphicsVertex;
    };

    struct ObjectInfo{
        //int Count = 0;
        struct ModelInfo{
            //std::vector<std::string> *Names = NULL;
            //std::vector<int> *List = NULL; 
            std::unique_ptr<std::vector<std::string>> Names; //models to be loaded
            std::unique_ptr<std::vector<int>> List; //list[i]=j: the i'th object use j'th model
            std::unique_ptr<std::vector<float>> ScaleList; 
        }Model;
        struct TextureInfo{
            //std::vector<std::pair<std::string, bool>> *Names = NULL;
            //std::vector<int> *List = NULL; 
            std::unique_ptr<std::vector<TextureAttributeInfo>> Attributes; 
            std::unique_ptr<std::vector<std::vector<int>>> List; //list[i]=j: the i'th object use j'th texture
        }Texture;
        struct PipelineInfo{
            std::unique_ptr<std::vector<std::string>> VertexShader;
            std::unique_ptr<std::vector<std::string>> FragmentShader;
            std::unique_ptr<std::vector<std::string>> ComputeShader;
            std::unique_ptr<std::vector<int>> GraphicsList; //list[i]=j: the i'th object use j'th pipeline
        }Pipeline;
    };

    struct AppInfo{
        ObjectInfo Object;
        RenderInfo Render;
        UniformInfo Uniform;
        FeatureInfo Feature;
        BufferInfo Buffer;
    }appInfo;

    void SetApplicationProperty(AppInfo &appInfo);
};



#endif
