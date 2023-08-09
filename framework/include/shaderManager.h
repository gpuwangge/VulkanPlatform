#ifndef H_SHADERMANAGER
#define H_SHADERMANAGER

#include "common.h"
#include "context.h"

class CShaderManager final{
public:
    CShaderManager();
    ~CShaderManager();

    VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

    void InitVertexShader(const std::string shaderName);
    void InitFragmentShader(const std::string shaderName);

    //bool bEnablePushConstant;
    VkPushConstantRange pushConstantRange;
    template<typename T>
    void CreatePushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset){
        //bEnablePushConstant = true;
        pushConstantRange.stageFlags = shaderStageFlagBits;
        pushConstantRange.offset = offset;
        pushConstantRange.size = sizeof(T);
    }

    void Destroy();

private:
    std::vector<char> readFile(const std::string& filename);
    void InitSpirVShader(const std::string shaderName, VkShaderModule *pShaderModule);

    CDebugger * debugger;
};

#endif