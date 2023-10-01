#include "../include/androidLogManager.h"

#define LOG_TAG "VULKAN_PLATFORM"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

CAndroidLogManager::CAndroidLogManager(){

}

CAndroidLogManager::~CAndroidLogManager(){
    
}


void CAndroidLogManager::printLog(std::string s){
    //__android_log_print(ANDROID_LOG_VERBOSE, "LogManager", "%s", s.c_str());
    LOGV("%s", s.c_str());
}

void CAndroidLogManager::printLog(std::string s, int n){
    //__android_log_print(ANDROID_LOG_VERBOSE, "LogManager", s.c_str(), n);
    LOGV(s.c_str(), n);
}

void CAndroidLogManager::printLog(std::string s, float n){
    //__android_log_print(ANDROID_LOG_VERBOSE, "LogManager", s.c_str(), n);
    LOGV(s.c_str(), n);
}

void CAndroidLogManager::printLog(std::string s0, std::string s1){
    //__android_log_print(ANDROID_LOG_VERBOSE, "LogManager", s0.c_str(), s1.c_str());
    LOGV(s0.c_str(), s1.c_str());
}