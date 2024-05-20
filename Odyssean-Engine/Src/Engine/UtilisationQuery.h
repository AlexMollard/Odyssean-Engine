#pragma once
#ifdef _WIN32
#include <chrono>
#include <pdh.h>

#pragma comment(lib, "pdh.lib")

class UtilisationQuery
{
public:
	UtilisationQuery(const wchar_t* counterPath);
	double GetUtilisation();

	static UtilisationQuery cpuUtilisation;
	static UtilisationQuery gpuUtilisation;
	static UtilisationQuery physicalMemory;
	static UtilisationQuery virtualMemory;
	static UtilisationQuery diskReadTime;
	static UtilisationQuery diskWriteTime;
	static UtilisationQuery networkBytesSent;
	static UtilisationQuery networkBytesReceived;

private:
	PDH_HQUERY query;
	PDH_HCOUNTER counter;
	PDH_FMT_COUNTERVALUE counterVal;
	const wchar_t* counterPath;
	bool queryCompleted = false;
	double cachedValue  = 0.0;
	std::chrono::steady_clock::time_point lastQueryTime;
};

#else // Probably Linux

#endif