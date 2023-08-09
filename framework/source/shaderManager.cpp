#include "shaderManager.h"

CShaderManager::CShaderManager(){
    debugger = new CDebugger("../logs/shaderManager.log");
    //bEnablePushConstant = false;
}
CShaderManager::~CShaderManager(){if (!debugger) delete debugger;}

void CShaderManager::InitVertexShader(const std::string shaderName){
    InitSpirVShader(shaderName, &vertShaderModule);
}
void CShaderManager::InitFragmentShader(const std::string shaderName){
    InitSpirVShader(shaderName, &fragShaderModule);
}

void CShaderManager::InitSpirVShader(const std::string shaderName, VkShaderModule *pShaderModule){
    HERE_I_AM("InitSpirVShader");

    auto shaderCode = readFile(shaderName.c_str());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(CContext::GetHandle().GetLogicalDevice(), &createInfo, PALLOCATOR, pShaderModule);
    REPORT("vkCreateShaderModule");
    debugger->writeMSG("Shader Module '%s' successfully loaded\n", shaderName.c_str());
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