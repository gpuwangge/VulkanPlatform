#include "../include/androidManager.h"
 
CAndroidManager::CAndroidManager(){}
CAndroidManager::~CAndroidManager(){}

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
    AAsset* file = AAssetManager_open(assetManager, assetName, AASSET_MODE_BUFFER);
    //ASSERT(assetDescriptor, "%s does not exist in %s", assetName.c_str(), __FUNCTION__);
    size_t fileLength = AAsset_getLength(file);

    buf.resize(fileLength);
    int64_t readSize = AAsset_read(file, buf.data(), buf.size());

    AAsset_close(file);
    return (readSize == buf.size());
}