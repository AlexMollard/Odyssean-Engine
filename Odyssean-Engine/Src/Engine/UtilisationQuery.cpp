#include "pch.h"

#include "UtilisationQuery.h"
#ifdef _WIN32
#include <string>
#include <locale>
#include <codecvt>

// Function to convert std::wstring to std::string
std::string wstring_to_string(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

// Define static members
UtilisationQuery UtilisationQuery::cpuUtilisation(L"\\Processor(_Total)\\% Processor Time");
UtilisationQuery UtilisationQuery::gpuUtilisation(L"\\GPU Engine(*)\\Utilization Percentage");
UtilisationQuery UtilisationQuery::physicalMemory(L"\\Memory\\% Committed Bytes In Use");
UtilisationQuery UtilisationQuery::virtualMemory(L"\\Paging File(_Total)\\% Usage");
UtilisationQuery UtilisationQuery::diskReadTime(L"\\PhysicalDisk(_Total)\\% Disk Read Time");
UtilisationQuery UtilisationQuery::diskWriteTime(L"\\PhysicalDisk(_Total)\\% Disk Write Time");
UtilisationQuery UtilisationQuery::networkBytesSent(L"\\Network Interface(*)\\Bytes Sent/sec");
UtilisationQuery UtilisationQuery::networkBytesReceived(L"\\Network Interface(*)\\Bytes Received/sec");

 UtilisationQuery::UtilisationQuery(const wchar_t* counterPath) : counterPath(counterPath)
{
	S_WARN(PdhOpenQuery(NULL, NULL, &query) == ERROR_SUCCESS, "PdhOpenQuery failed.");
	S_WARN(PdhAddEnglishCounter(query, counterPath, NULL, &counter) == ERROR_SUCCESS, std::format("PdhAddCounter failed for path: {}", wstring_to_string(counterPath)));
	S_WARN(PdhCollectQueryData(query) == ERROR_SUCCESS, "PdhCollectQueryData failed on initialization.");
}

double UtilisationQuery::GetUtilisation()
{
	auto currentTime = std::chrono::steady_clock::now();
	if (!queryCompleted || std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastQueryTime).count() >= 1)
	{
		lastQueryTime = currentTime;
		if (!queryCompleted)
		{
			S_WARN(PdhCollectQueryData(query) == ERROR_SUCCESS, "PdhCollectQueryData failed.");
			S_WARN(PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal) == ERROR_SUCCESS, "PdhGetFormattedCounterValue failed.");
			cachedValue    = counterVal.doubleValue;
			queryCompleted = true;
		}

		S_WARN(PdhCollectQueryData(query) == ERROR_SUCCESS, "PdhCollectQueryData failed.");
		S_WARN(PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal) == ERROR_SUCCESS, "PdhGetFormattedCounterValue failed.");
		cachedValue = counterVal.doubleValue;
	}

	return cachedValue;
}
#else // Probably Linux

#endif