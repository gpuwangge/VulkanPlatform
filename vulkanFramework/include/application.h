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
#include "timer.h"

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
    CTextManager textManager;

    //static int focusObjectId;
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

    //static Camera lightCameras[2];
    //static std::vector<Camera> lightCameras;

    //Camera lightCameras[2]; //works
    std::vector<Camera> lightCameras; 

    static bool NeedToExit;
    static bool NeedToPause;

    static bool PrintFPS;

    /*Clean up Functions*/
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    //Make elapseTime and deltaTime public so that user(sample) can access them
    double elapseTime = 0;
    double deltaTime = 0;

    std::string m_sampleName;
    YAML::Node config;

    /******************
    * Core Functions
    ******************/
    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize(); //use this to call sample initialization
    //void initialize(); //base: create sync object, destroy shader resource
    virtual void update(); //base: update time, frame id, camera and ubo
    virtual void recordGraphicsCommandBuffer_renderpassMainscene();
    virtual void recordGraphicsCommandBuffer_renderpassShadowmap(int renderpassIndex);
    virtual void recordComputeCommandBuffer();
    virtual void postUpdate();

    /*Pure virtual function(=0): base class not implment, derived class must implement*/
    //NA

    /******************
    * Helper Functions
    ******************/
    void ReadFeatures();
    void ReadUniforms();
    void ReadAttachments();
    void ReadSubpasses();
    void ReadResources();
    void CreateUniformDescriptors(bool b_uniform_graphics, bool b_uniform_compute);
    void CreatePipelines();
    void ReadRegisterObjects();
    void ReadLightings();
    void ReadCameras();
    void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);  
    
    
    /*************
     * APP INFO
     *******/
    struct FeatureInfo{
        //bool b_feature_graphics_depth_test = false;
        //bool b_feature_graphics_msaa = false;
        bool b_feature_graphics_48pbt = false;
        bool b_feature_graphics_push_constant = false;
        bool b_feature_graphics_blend = false;
        bool b_feature_graphics_rainbow_mipmap = false;
        int feature_graphics_pipeline_skybox_id = -1;
        int feature_graphics_observe_attachment_id = -1;
        bool b_feature_graphics_fps = false;
    };
    // struct AttachmentInfo{
    //     bool bAttachmentDepthLight;
    //     bool bAttachmentDepthCamera;
    //     bool bAttachmentColorResovle;
    //     bool bAttachmentColorPresent;
    // };
    struct UniformInfo{
        bool b_uniform_graphics_custom = false;
        bool b_uniform_graphics_mvp = false;
        bool b_uniform_graphics_vp = false;
        bool b_uniform_graphics_lighting = false;
        bool b_uniform_graphics_depth_image_sampler = false;
        bool b_uniform_graphics_lightdepth_image_sampler = false;
        bool b_uniform_graphics_lightdepth_image_sampler_hardware = false;
        bool b_uniform_compute_custom = false;
        bool b_uniform_compute_storage = false;
        bool b_uniform_compute_swapchain_storage = false;
        bool b_uniform_compute_texture_storage = false;
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
        FeatureInfo Feature;
        //AttachmentInfo Attachment;
        UniformInfo Uniform;
        std::unique_ptr<std::vector<std::string>> VertexShader;
        std::unique_ptr<std::vector<std::string>> FragmentShader;
        //std::unique_ptr<std::vector<bool>> EnableSamplerCountOne; 
        //std::unique_ptr<std::vector<bool>> EnableDepthBias;
        std::unique_ptr<std::vector<bool>> RenderPassShadowmap;
        std::unique_ptr<std::vector<int>> Subpass;
        std::unique_ptr<std::vector<std::string>> ComputeShader;
        CRenderer::RenderModes RenderMode = CRenderer::GRAPHICS;
        VertexStructureTypes VertexBufferType = (VertexStructureTypes)NULL;
    }appInfo;
};



#endif
