#include "logicalDevice.h"

CLogicalDevice::CLogicalDevice(){
    debugger = new CDebugger("../logs/logicalDeviceLog.txt");
}

CLogicalDevice::~CLogicalDevice(){
    if (!debugger) delete debugger;
}

