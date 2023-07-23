#ifndef H_LOGICAL_DEVICE
#define H_LOGICAL_DEVICE

#include "debugger.h"
#include "common.h"

class CLogicalDevice{
public:
    CLogicalDevice();

    ~CLogicalDevice();

    VkDevice handle{VK_NULL_HANDLE};
    VkDevice getHandle() const{ return handle;}

    // void createLogicalDevices(CPhysicalDevice &physicalDevice){

    // }

    CDebugger * debugger;
};

#endif
