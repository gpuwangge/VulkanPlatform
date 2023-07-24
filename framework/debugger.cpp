#include "debugger.h"

	CDebugger::CDebugger(const std::string&  debugFilename) {
		Verbose = true;

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
		fprintf(FpDebug, "Width = %d ; Height = %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);

	}

	void CDebugger::writeMSG(std::string s) {
		if(Verbose) fprintf(FpDebug, s.c_str());
	}

	void CDebugger::writeMSG(std::string s0, std::string s1) {
		if (Verbose) fprintf(FpDebug, s0.c_str(), s1.c_str());
	}

	void CDebugger::writeMSG(std::string s, uint32_t n) {
		if (Verbose) fprintf(FpDebug, s.c_str(), n);
	}

	void CDebugger::printVkError(VkResult result, std::string prefix){
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


	void CDebugger::flush() {
		fflush(FpDebug);
	}
