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
        glm::vec3 length;
        glm::vec3 lengthMax;
        glm::vec3 lengthMin;
    };
    std::vector<CCustomModel3D> customModels3D; //use custom 3d id
    void CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D);

    class CCustomModel2D{
    public:
        std::vector<Vertex2D> vertices;
        std::vector<uint32_t> indices;
        glm::vec3 length;
        glm::vec3 lengthMax;
        glm::vec3 lengthMin;
    };
    std::vector<CCustomModel2D> customModels2D; //use custom 2d id
    void CreateCustomModel2D(std::vector<Vertex2D> &vertices2D);

    std::vector<glm::vec3> modelLengths; //use global model id
    std::vector<glm::vec3> modelLengthsMax;
    std::vector<glm::vec3> modelLengthsMin;

    void LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);
};

#endif
