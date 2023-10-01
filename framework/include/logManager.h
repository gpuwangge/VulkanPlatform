#ifndef H_LOGMANAGER
#define H_LOGMANAGER

#include "common.h"

#ifndef ANDROID
    #include "..\\..\\windowsFramework\\include\\windowsLogManager.h"
#else
    #include "..\\..\\androidFramework\\include\\androidLogManager.h"
#endif
class CLogManager {
public:
	CLogManager();
    ~CLogManager();
//#ifndef ANDROID
    void print(std::string s);
    void print(std::string s, int n); //uint32_t
    void print(std::string s, float n);
    void print(std::string s0, std::string s1);
//#endif

private:
#ifndef ANDROID
    CWindowsLogManager windowsLogManager;
#else
    CAndroidLogManager androidLogManager;
#endif
};

#endif