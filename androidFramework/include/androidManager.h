#ifndef H_ANDROIDMANAGER
#define H_ANDROIDMANAGER

#include "..\\..\\framework\\include\\common.h"
#include <android/asset_manager.h>

class CAndroidManager final{
public:
    CAndroidManager();
    ~CAndroidManager();

    AAssetManager *assetManager;
    bool AssetReadFile(const char *assetName, std::vector<uint8_t>& buf); //std::string& assetName

    VkShaderModule createShaderModule(const std::vector<uint8_t> &code);
};

#endif
