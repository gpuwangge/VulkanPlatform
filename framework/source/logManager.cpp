#include "../include/logManager.h"



CLogManager::CLogManager(){
#ifndef ANDROID
    windowsLogManager.createLogFile("../logs/context.log");
#endif
}

CLogManager::~CLogManager(){
    
}

void CLogManager::writeLog(std::string s){
#ifndef ANDROID
    windowsLogManager.writeMSG(s);
#else
    __android_log_print(ANDROID_LOG_VERBOSE, "LogManager", " %s", s.c_str());
#endif
}
