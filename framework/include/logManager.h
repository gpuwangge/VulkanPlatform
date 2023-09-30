#ifndef H_LOGMANAGER
#define H_LOGMANAGER

#include "common.h"

#ifndef ANDROID
    #include "..\\..\\windowsFramework\\include\\windowsLogManager.h"
#else
    #include <android/log.h>
    #include "..\\..\\androidFramework\\include\\androidLogManager.h"
#endif
class CLogManager {
public:
	CLogManager();
    ~CLogManager();
//#ifndef ANDROID
    void writeLog(std::string s);
//#endif

private:
#ifndef ANDROID
    CWindowsLogManager windowsLogManager;
#else
    CAndroidLogManager androidLogManager;
#endif
};

#endif