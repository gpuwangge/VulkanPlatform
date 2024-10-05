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

#ifndef ANDROID
#include "..\\..\\windowsFramework\\include\\glfwManager.h"
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
    //std::vector<CGraphicsDescriptorManager> descriptors;//to be remove
    CGraphicsDescriptorManager graphicsDescriptorManager;
    CComputeDescriptorManager computeDescriptorManager;
    //CTextureDescriptor textureDescriptor;
    CRenderer renderer;
    //std::vector<CTextureImage> textureImages; //temp
    CModelManager modelManager;
    //std::vector<CTextureImage> textureImages1; //temp
    //std::vector<CTextureImage> textureImages2; //temp
    CTextureManager textureManager;

    //std::chrono::_V2::system_clock::time_point lastTime;

    //VertexStructureTypes vertexStructuretypes;
    std::vector<CObject> objectList;
    //int ObjectCount = 0;
	//std::vector<int> modelList = {}; 
	//std::vector<int> textureList = {}; 

    void CleanUp();

    void UpdateRecordRender();
    
#ifndef ANDROID
    CGLFWManager glfwManager;
    void run();
#endif

    //for static class member. But can not define and init them in the header file!
    static Camera mainCamera; 
    static bool NeedToExit;

    /*Clean up Functions*/
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    
    float durationTime = 0;
    float deltaTime = 0;

    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize(); //base: create sync object, destroy shader resource
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

    struct RenderInfo{
        CRenderer::RenderModes Mode = CRenderer::RENDER_GRAPHICS_Mode;
    };
    struct UniformInfo{
        bool EnableGraphicsVP = false;
        bool EnableGraphicsMVP = false;
        struct GraphicsCustomInfo{
            //bool Enable = false;
            VkDeviceSize Size = 0;
            VkDescriptorSetLayoutBinding Binding;
        }GraphicsCustom;
        struct GraphicsSamplerInfo{
            //bool Enable = false;
            int Count = 0;
            bool UseMultiSamplerForOneObject = false;
        }GraphicsSampler;
        //--------------------------------------
        struct ComputeCustomInfo{
            //bool Enable = false;
            VkDeviceSize Size = 0;
            VkDescriptorSetLayoutBinding Binding;
        }ComputeCustom;
        struct ComputeStorageBufferInfo{
            //bool Enable = false;
            VkDeviceSize Size = 0;
            VkBufferUsageFlags Usage;
        }ComputeStorageBuffer;
        bool EnableComputeStorageImage = false;
        bool EnableComputeStorageImageSwapChain = false;
    };
    struct FeatureInfo{
        bool EnableGraphicsDepthTest = false;
        bool EnableGraphicsMSAA = false;
        bool EnableGraphics48BPT = false;
        bool EnableGraphicsPushConstant = false;
        bool EnableGraphicsBlend = false;
        bool EnableGraphicsRainbowMipmap = false;
    };
    struct BufferInfo{
        struct GraphicsVertexInfo{
            //bool Enable = false;
            VertexStructureTypes StructureType = (VertexStructureTypes)NULL;
            std::vector<Vertex3D> *Vertices3D = NULL;
            std::vector<uint32_t> *Indices3D = NULL;
            std::vector<Vertex2D> *Vertices2D = NULL;
        }GraphicsVertex;
    };
    struct ShaderInfo{
        std::string Vertex;
        std::string Fragment;
        std::string Compute;
    };

    struct ObjectInfo{
        int Count = 0;
        struct ModelInfo{
            //std::vector<std::string> *Names = NULL;
            //std::vector<int> *List = NULL; 
            std::unique_ptr<std::vector<std::string>> Names;
            std::unique_ptr<std::vector<int>> List; //the i'th object use i'th model
        }Model;
        struct TextureInfo{
            //std::vector<std::pair<std::string, bool>> *Names = NULL;
            //std::vector<int> *List = NULL; 
            std::unique_ptr<std::vector<std::pair<std::string, bool>>> Names; //first: textureName, second: mipmap, //the i'th object use i'th texture
            std::unique_ptr<std::vector<int>> List;
        }Texture;
    };

    struct AppInfo{
        ObjectInfo Object;
        RenderInfo Render;
        ShaderInfo Shader;
        UniformInfo Uniform;
        FeatureInfo Feature;
        BufferInfo Buffer;
    }appInfo;

    void SetApplicationProperty(AppInfo &appInfo);
};



#endif