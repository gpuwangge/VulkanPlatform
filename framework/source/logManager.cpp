#include "../include/logManager.h"



CLogManager::CLogManager(){
    windowsLogManager.createLogFile("../logs/context.log");
}

CLogManager::~CLogManager(){
    
}

void CLogManager::writeLog(std::string s){
    windowsLogManager.writeMSG(s);
}