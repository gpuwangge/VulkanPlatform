#ifndef H_APPLICATION
#define H_APPLICATION

#include "common.h"
#include "camera.hpp"
#include "instance.h"
#include "swapchain.h"
#include "dataBuffer.hpp"
#include "context.h"
#include "graphicsDescriptor.h"
#include "computeDescriptor.h"
#include "renderProcess.h"
#include "shaderManager.h"
#include "renderer.h"
#include "texture.h" //this includes imageManager.h
#include "modelManager.h"
#include "object.h"
#include "light.h"

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

    static int focusObjectId;
    static std::vector<CObject> objects;
    static std::vector<CLight> lights;

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

    /*Clean up Functions*/
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    
    float durationTime = 0;
    float deltaTime = 0;

    std::string m_sampleName;
    YAML::Node config;
    
    /******************
    * Core Functions
    ******************/
    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize(); //use this to call sample initialization
    //void initialize(); //base: create sync object, destroy shader resource
    virtual void update(); //base: update time, frame id, camera and ubo
    virtual void recordGraphicsCommandBuffer();
    virtual void recordComputeCommandBuffer();
    virtual void postUpdate();

    /*Pure virtual function(=0): base class not implment, derived class must implement*/
    //NA

    /******************
    * Helper Functions
    ******************/
    void ReadFeatures(bool& b_feature_graphics_depth_test, bool& b_feature_graphics_msaa, bool& b_feature_graphics_48pbt,
                    bool& b_feature_graphics_push_constant, bool& b_feature_graphics_blend, bool& b_feature_graphics_rainbow_mipmap,
                    int& feature_graphics_pipeline_skybox_id);
    void ReadUniforms(bool& b_uniform_graphics_custom, bool&b_uniform_graphics_mvp, bool&b_uniform_graphics_vp, bool&b_uniform_graphics_lighting,
                    bool&b_uniform_compute_custom, bool&b_uniform_compute_storage, bool&b_uniform_compute_swapchain_storage, bool&b_uniform_compute_texture_storage);
    void ReadResources(bool b_feature_graphics_48pbt, bool b_feature_graphics_rainbow_mipmap);
    void CreateUniformDescriptors(bool b_uniform_graphics, bool b_uniform_compute, 
                                bool b_uniform_graphics_custom, bool b_uniform_compute_custom,
                                bool b_uniform_compute_swapchain_storage, bool b_uniform_compute_texture_storage);
    void CreatePipelines();
    void ReadRegisterObjects();
    void ReadLightings();
    void ReadMainCamera();
    void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);  
    
    
    /*************
     * APP INFO
     *******/
    struct UniformInfo{
        struct GraphicsCustomInfo{
            VkDeviceSize Size = 0;
            VkDescriptorSetLayoutBinding Binding;
        }GraphicsCustom;
        struct ComputeCustomInfo{
            VkDeviceSize Size = 0;
            VkDescriptorSetLayoutBinding Binding;
        }ComputeCustom;
        struct ComputeStorageBufferInfo{
            VkDeviceSize Size = 0;
            VkBufferUsageFlags Usage;
        }ComputeStorageBuffer;
    };

    struct AppInfo{
        UniformInfo Uniform;
        std::unique_ptr<std::vector<std::string>> VertexShader;
        std::unique_ptr<std::vector<std::string>> FragmentShader;
        std::unique_ptr<std::vector<std::string>> ComputeShader;
        CRenderer::RenderModes RenderMode = CRenderer::RENDER_GRAPHICS_Mode;
        VertexStructureTypes VertexBufferType = (VertexStructureTypes)NULL;
    }appInfo;
};



#endif
