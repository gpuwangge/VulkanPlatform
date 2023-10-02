#include "windowsLogManager.h"

CWindowsLogManager::CWindowsLogManager() {
	Verbose = true;
}

void CWindowsLogManager::createLogFile(const std::string&  debugFilename){
#ifdef _WIN32
	errno_t err = fopen_s(&FpDebug, debugFilename.c_str(), "w");
	if (err != 0)
	{
		fprintf(stderr, "Cannot open debug print file '%s'\n", debugFilename.c_str());
		FpDebug = stderr;
	}
#else
	FpDebug = fopen(DEBUGFILE, "w");
	if (FpDebug == NULL)
	{
		fprintf(stderr, "Cannot open debug print file '%s'\n", DEBUGFILE);
		FpDebug = stderr;
	}
#endif
	//fprintf(FpDebug, "Width = %d ; Height = %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
}

void CWindowsLogManager::printLog(std::string s) {
	if(Verbose) fprintf(FpDebug, "%s", s.c_str());
}

void CWindowsLogManager::printLog(std::string s, int n) {
	if (Verbose) fprintf(FpDebug, s.c_str(), n);
}

void CWindowsLogManager::printLog(std::string s, float n) {
	if (Verbose) fprintf(FpDebug, s.c_str(), n);
}

void CWindowsLogManager::printLog(std::string s, float n0, float n1){
	if (Verbose) fprintf(FpDebug, s.c_str(), n0, n1);
}

void CWindowsLogManager::printLog(std::string s0, std::string s1) {
	if (Verbose) fprintf(FpDebug, s0.c_str(), s1.c_str());
}

void CWindowsLogManager::printLog(std::string s, float *n, int size){
	if(Verbose) fprintf(FpDebug, "%s", s.c_str());
	for(int i = 0; i < size; i++) fprintf(FpDebug, "%f, ", n[i]);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CWindowsLogManager::printVkError(VkResult result, std::string prefix){
	if (Verbose  &&  result == VK_SUCCESS)
	{
		fprintf(FpDebug, "%s: %s\n", prefix.c_str(), "Successful");
		fflush(FpDebug);
		return;
	}

	const int numErrorCodes = sizeof(ErrorCodes) / sizeof(struct errorcode);
	std::string meaning = "";
	for (int i = 0; i < numErrorCodes; i++)
	{
		if (result == ErrorCodes[i].resultCode)
		{
			meaning = ErrorCodes[i].meaning;
			break;
		}
	}

	fprintf(FpDebug, "\n%s: %s\n", prefix.c_str(), meaning.c_str());
	fflush(FpDebug);
}


void CWindowsLogManager::flush() {
	fflush(FpDebug);
}
