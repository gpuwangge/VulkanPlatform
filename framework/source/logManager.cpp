#include "../include/logManager.h"



CLogManager::CLogManager(){
#ifndef ANDROID
    windowsLogManager.createLogFile("../logs/context.log");
#endif
}

CLogManager::~CLogManager(){
    
}

void CLogManager::print(std::string s){
#ifndef ANDROID
    windowsLogManager.printLog(s);
#else
    androidLogManager.printLog(s);
#endif
}

void CLogManager::print(std::string s, int n){
#ifndef ANDROID
    windowsLogManager.printLog(s, n);
#else
    androidLogManager.printLog(s, n);
#endif    
}

void CLogManager::print(std::string s, float n){
#ifndef ANDROID
    windowsLogManager.printLog(s, n);
#else
    androidLogManager.printLog(s, n);
#endif    
}

void CLogManager::print(std::string s0, std::string s1){
#ifndef ANDROID
    windowsLogManager.printLog(s0, s1);
#else
    androidLogManager.printLog(s0, s1);
#endif 
}