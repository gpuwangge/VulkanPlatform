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

    void Destroy();

private:
    std::vector<char> readFile(const std::string& filename);
    void InitSpirVShader(const std::string shaderName, VkShaderModule *pShaderModule);

    CDebugger * debugger;
};

#endif