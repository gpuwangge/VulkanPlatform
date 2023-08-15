#include "../include/androidManager.h"
 
CAndroidManager::CAndroidManager(){}

CAndroidManager::~CAndroidManager(){}


// VkShaderModule CAndroidManager::InitVertexShader(){
//     //auto vertShaderCode = LoadBinaryFileToVector("shaders/shader.vert.spv", assetManager);
//     std::vector<uint8_t> fileBits;
//     AssetReadFile("shaders/shader.vert.spv", fileBits);
//     return createShaderModule(fileBits);
// }

// VkShaderModule CAndroidManager::InitFragmentShader(){
//     //auto fragShaderCode = LoadBinaryFileToVector("shaders/shader.frag.spv", assetManager);
//     std::vector<uint8_t> fileBits;
//     AssetReadFile("shaders/shader.frag.spv", fileBits);
//     return createShaderModule(fileBits);
// }

// std::vector<uint8_t> CAndroidManager::LoadBinaryFileToVector(const char *file_path,
//                                             AAssetManager *assetManager) {
//     std::vector<uint8_t> file_content;
//     assert(assetManager);
//     AAsset *file =
//             AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
//     size_t file_length = AAsset_getLength(file);

//     file_content.resize(file_length);

//     AAsset_read(file, file_content.data(), file_length);
//     AAsset_close(file);
//     return file_content;
// }

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

bool CAndroidManager::AssetReadFile(const char *assetName, std::vector<uint8_t>& buf) {
    //if (!assetName.length()) return false;
    AAsset* assetDescriptor = AAssetManager_open(assetManager, assetName, AASSET_MODE_BUFFER);
    //ASSERT(assetDescriptor, "%s does not exist in %s", assetName.c_str(), __FUNCTION__);
    size_t fileLength = AAsset_getLength(assetDescriptor);

    buf.resize(fileLength);
    int64_t readSize = AAsset_read(assetDescriptor, buf.data(), buf.size());

    AAsset_close(assetDescriptor);
    return (readSize == buf.size());
}