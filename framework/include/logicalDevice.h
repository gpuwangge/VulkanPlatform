#ifndef H_LOGICAL_DEVICE
#define H_LOGICAL_DEVICE


#include "common.h"

class CLogicalDevice{

public:
    CLogicalDevice();

    ~CLogicalDevice();

    VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;

    //VkDevice handle{VK_NULL_HANDLE};
    //VkDevice getHandle() const{ return handle;}

    //CDebugger * debugger;
};

#endif
