#ifndef H_LOGMANAGER
#define H_LOGMANAGER

#include "..\\..\\windowsFramework\\include\\windowsLogManager.h"
#include "..\\..\\androidFramework\\include\\androidLogManager.h"

class CLogManager {
public:
	CLogManager();
    ~CLogManager();

    void writeLog(std::string s);

private:
    CWindowsLogManager windowsLogManager;
    //CAndroidLogManager androidLogManager;
};

#endif