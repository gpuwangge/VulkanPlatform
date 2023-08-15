#ifndef H_ANDROIDMANAGER
#define H_ANDROIDMANAGER

#include "..\\..\\framework\\include\\context.h"
#include <android/asset_manager.h>

class CAndroidManager final{
public:
    CAndroidManager();
    ~CAndroidManager();

    AAssetManager *assetManager;
    //bool AssetReadFile(std::string& assetName, std::vector<uint8_t>& buf);
    bool AssetReadFile(const char *assetName, std::vector<uint8_t>& buf);

    //VkShaderModule InitVertexShader();
    //VkShaderModule InitFragmentShader();
    VkShaderModule createShaderModule(const std::vector<uint8_t> &code);
    
private:
    //std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path, AAssetManager *assetManager);  
};

#endif
