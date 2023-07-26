#include "logicalDevice.h"


CLogicalDevice::CLogicalDevice(){
    //printf("logicalDevice\n");
    debugger = new CDebugger("../logs/logicalDeviceLog.txt");
}

CLogicalDevice::~CLogicalDevice(){
    if (!debugger) delete debugger;
}

