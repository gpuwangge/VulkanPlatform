#ifndef H_MODELMANAGER
#define H_MODELMANAGER

#include "common.h"
#include "dataBuffer.hpp"

 #ifdef ANDROID
// #include "..\\..\\androidFramework\\include\\androidManager.h"
#include "context.h"
 #endif

class CModelManager final{

public:
    CModelManager();
    ~CModelManager();

    void LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);
};

#endif
