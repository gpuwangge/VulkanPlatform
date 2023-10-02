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

void CLogManager::print(std::string s, int n0, int n1){
#ifndef ANDROID
    windowsLogManager.printLog(s, n0, n1);
#else
    androidLogManager.printLog(s, n0, n1);
#endif    
}

void CLogManager::print(std::string s, float n){
#ifndef ANDROID
    windowsLogManager.printLog(s, n);
#else
    androidLogManager.printLog(s, n);
#endif    
}

void CLogManager::print(std::string s, float n0, float n1){
#ifndef ANDROID
    windowsLogManager.printLog(s, n0, n1);
#else
    androidLogManager.printLog(s, n0, n1);
#endif  
}

void CLogManager::print(std::string s0, std::string s1){
#ifndef ANDROID
    windowsLogManager.printLog(s0, s1);
#else
    androidLogManager.printLog(s0, s1);
#endif 
}

void CLogManager::print(std::string s, float *n, int size){
#ifndef ANDROID
    windowsLogManager.printLog(s, n, size);
#else
    androidLogManager.printLog(s, n, size);
#endif 
}