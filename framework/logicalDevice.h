#ifndef H_LOGICAL_DEVICE
#define H_LOGICAL_DEVICE


#include "common.h"

class CLogicalDevice{

public:
    CLogicalDevice();

    ~CLogicalDevice();

    VkDevice logicalDevice;//04
	VkQueue graphicsQueue;//04
	VkQueue presentQueue;//04
	VkQueue computeQueue;//04

    //VkDevice handle{VK_NULL_HANDLE};
    //VkDevice getHandle() const{ return handle;}

    CDebugger * debugger;
};

#endif
