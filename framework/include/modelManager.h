#ifndef H_MODELMANAGER
#define H_MODELMANAGER


#include "common.h"
#include "dataStructure.hpp"

class CModelManager final{

public:
    CModelManager();
    ~CModelManager();

    void LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);
};

#endif
