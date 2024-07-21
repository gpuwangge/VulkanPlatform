#ifndef H_OBJECT
#define H_OBJECT
#include "common.h"
#include "context.h"
#include "dataBuffer.hpp"

class CObject {
public:
    std::vector<Vertex3D> vertices3D;
	std::vector<uint32_t> indices3D;
    unsigned int id;

    CObject(){
        id = 0;
    }
};

#endif