#include "../include/shaderManager.h"

CShaderManager::CShaderManager(){
    //debugger = new CDebugger("../logs/shaderManager.log");
    //bEnablePushConstant = false;
}
CShaderManager::~CShaderManager(){
    //if (!debugger) delete debugger;
}

void CShaderManager::CreateVertexShader(const std::string shaderName){
#ifndef ANDROID
    InitSpirVShader(SHADER_PATH + shaderName, &vertShaderModule);
#else
    std::vector<uint8_t> fileBits;
    std::string tmp = ANDROID_SHADER_PATH + shaderName;
    std::string fullShaderName;
    for(int i = 0, j = 0; i < tmp.size(); i++){ //convert sample/vert.spv into sample/shader.vert.spv
        fullShaderName[j++] = tmp[i];
        if(tmp[i] == '/'){
            fullShaderName = fullShaderName + "shader.";
            j += 7;
        }
    }
    androidManager.AssetReadFile(fullShaderName.c_str(), fileBits);
    vertShaderModule = androidManager.createShaderModule(fileBits);
#endif    
}
void CShaderManager::CreateFragmentShader(const std::string shaderName){
#ifndef ANDROID
    InitSpirVShader(SHADER_PATH + shaderName, &fragShaderModule);
#else
    std::vector<uint8_t> fileBits;
    std::string tmp = ANDROID_SHADER_PATH + shaderName;
    std::string fullShaderName;
    for(int i = 0, j = 0; i < tmp.size(); i++){ 
        fullShaderName[j++] = tmp[i];
        if(tmp[i] == '/'){
            fullShaderName = fullShaderName + "shader.";
            j += 7;
        }
    }
    androidManager.AssetReadFile(fullShaderName.c_str(), fileBits);
    fragShaderModule = androidManager.createShaderModule(fileBits);
#endif      
}
void CShaderManager::CreateComputeShader(const std::string shaderName){
    InitSpirVShader(SHADER_PATH + shaderName, &computeShaderModule);
}


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