#include "../include/shaderManager.h"

CShaderManager::CShaderManager(){
    //debugger = new CDebugger("../logs/shaderManager.log");
    //bEnablePushConstant = false;
}
CShaderManager::~CShaderManager(){
    //if (!debugger) delete debugger;
}

#ifndef ANDROID
void CShaderManager::InitSpirVShader(const std::string shaderName, VkShaderModule *pShaderModule){
    auto shaderCode = readFile(shaderName.c_str());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(CContext::GetHandle().GetLogicalDevice(), &createInfo, PALLOCATOR, pShaderModule);
    //REPORT("vkCreateShaderModule");
    //debugger->writeMSG("Shader Module '%s' successfully loaded\n", shaderName.c_str());
}
#else
VkShaderModule CShaderManager::createShaderModule(const std::vector<uint8_t> &code) {
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

std::string CShaderManager::InsertString(std::string originalString, std::string insertString, char separator){
    std::string str1;
    std::string str2;
    for(int i = 0, j = 0; i < originalString.size(); i++){
        if(originalString[i] == separator) {
            str1.append(originalString, 0, i+1);
            str2.append(originalString, i+1, originalString.size() - i - 1);
        }
    }
    str1 = str1 + insertString + str2;
    return str1;
}
#endif

void CShaderManager::CreateVertexShader(const std::string shaderName){
#ifndef ANDROID
    InitSpirVShader(SHADER_PATH + shaderName, &vertShaderModule);
#else
    std::vector<uint8_t> fileBits;
    std::string fullShaderName = ANDROID_SHADER_PATH + InsertString(shaderName, "shader.", '/');
    CContext::GetHandle().androidManager.AssetReadFile(fullShaderName.c_str(), fileBits);
    vertShaderModule = createShaderModule(fileBits);
#endif    
}
void CShaderManager::CreateFragmentShader(const std::string shaderName){
#ifndef ANDROID
    InitSpirVShader(SHADER_PATH + shaderName, &fragShaderModule);
#else
    std::vector<uint8_t> fileBits;
    std::string fullShaderName = ANDROID_SHADER_PATH + InsertString(shaderName, "shader.", '/');
    CContext::GetHandle().androidManager.AssetReadFile(fullShaderName.c_str(), fileBits);
    fragShaderModule = createShaderModule(fileBits);
#endif      
}
void CShaderManager::CreateComputeShader(const std::string shaderName){
#ifndef ANDROID
    InitSpirVShader(SHADER_PATH + shaderName, &computeShaderModule);
#endif
}

std::vector<char> CShaderManager::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void CShaderManager::Destroy(){
    vkDestroyShaderModule(CContext::GetHandle().GetLogicalDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(CContext::GetHandle().GetLogicalDevice(), vertShaderModule, nullptr);
}