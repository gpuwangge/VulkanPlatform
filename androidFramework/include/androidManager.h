#ifndef H_ANDROIDMANAGER
#define H_ANDROIDMANAGER

#include "..\\..\\framework\\include\\context.h"
#include <android/asset_manager.h>

class CAndroidManager final{
public:
    CAndroidManager();
    ~CAndroidManager();

    AAssetManager *assetManager;

    VkShaderModule InitVertexShader();
    VkShaderModule InitFragmentShader();

    bool AssetReadFile(std::string& assetName, std::vector<uint8_t>& buf);
private:
    std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path, AAssetManager *assetManager);
    VkShaderModule createShaderModule(const std::vector<uint8_t> &code);
};

#endif
