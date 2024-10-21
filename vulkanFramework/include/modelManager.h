#ifndef H_MODELMANAGER
#define H_MODELMANAGER

#include "common.h"
#include "dataBuffer.hpp"

 #ifdef ANDROID
#include "context.h"
 #endif

class CModelManager final{

public:
    CModelManager();
    ~CModelManager();

    class CCustomModel3D{
    public:
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;
    };
    std::vector<CCustomModel3D> customModels3D;
    void CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D);

    class CCustomModel2D{
    public:
        std::vector<Vertex2D> vertices;
        std::vector<uint32_t> indices;
    };
    std::vector<CCustomModel2D> customModels2D;
    void CreateCustomModel2D(std::vector<Vertex2D> &vertices2D);

    void LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);
};

#endif
