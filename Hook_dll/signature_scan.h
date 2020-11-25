#pragma once

#include <Windows.h>
#include <Psapi.h>

class SigScan
{
public:
	// getting information about the executing module
	MODULEINFO getModuleInfo(char* szModule)
	{
		MODULEINFO moduleInfo = { 0 };
		HMODULE hModule = GetModuleHandle(szModule);
		if (hModule == 0) 
			return moduleInfo;
		GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO));
		return moduleInfo;
	}

	// find a signature/pattern in memory of another process
	DWORD findPattern(char* module, char* pattern, char* mask)
	{
		MODULEINFO moduleInfo = getModuleInfo(module);
		DWORD base = (DWORD)moduleInfo.lpBaseOfDll;
		DWORD size = (DWORD)moduleInfo.SizeOfImage;
		DWORD patternLength = (DWORD)strlen(mask);

		for (DWORD i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD j = 0; j < patternLength; j++) {
				found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
			}
			if (found) {
				return base + i;
			}
		}

		return NULL;
	}
};

