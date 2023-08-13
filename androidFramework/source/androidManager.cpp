#include "androidManager.h"
 
CAndroidManager::CAndroidManager(){}

CAndroidManager::~CAndroidManager(){}



/**********
 * 
 * Android Shader Help Functions
 * 
*/
VkShaderModule CAndroidManager::InitVertexShader(){
    auto vertShaderCode = LoadBinaryFileToVector("shaders/shader.vert.spv", assetManager);
    return createShaderModule(vertShaderCode);
}

VkShaderModule CAndroidManager::InitFragmentShader(){
    auto fragShaderCode = LoadBinaryFileToVector("shaders/shader.frag.spv", assetManager);
    return createShaderModule(fragShaderCode);
}

std::vector<uint8_t> CAndroidManager::LoadBinaryFileToVector(const char *file_path,
                                            AAssetManager *assetManager) {
    std::vector<uint8_t> file_content;
    assert(assetManager);
    AAsset *file =
            AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
    size_t file_length = AAsset_getLength(file);

    file_content.resize(file_length);

    AAsset_read(file, file_content.data(), file_length);
    AAsset_close(file);
    return file_content;
}

VkShaderModule CAndroidManager::createShaderModule(const std::vector<uint8_t> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();

    // Satisifies alignment requirements since the allocator
    // in vector ensures worst case requirements
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule shaderModule;
    vkCreateShaderModule( CContext::GetHandle().GetLogicalDevice(), &createInfo, nullptr, &shaderModule);
    //VkResult result = vkCreateShaderModule(CContext::GetHandle().GetLogicalDevice(), &createInfo, PALLOCATOR, pShaderModule);

    return shaderModule;
}


