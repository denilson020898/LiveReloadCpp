#include "LiveReload/LiveReload.h"

#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <time.h>

#ifdef NDEBUG
typedef void(*LiveCodePtr)();
#else
extern "C" LIVE_CODE_API void LiveCodeFunc();

struct LiveCode
{
	std::wstring DllPath;
	std::vector<std::string> ProcList;

	HMODULE Module = NULL;
	uint64_t LastWrite = 0;
	std::map<std::string, void*> Procs;
};

void Poll(LiveCode& liveCode)
{
	HANDLE handle = CreateFileW(liveCode.DllPath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	uint64_t lastWrite;
	if (GetFileTime(handle, NULL, NULL, (FILETIME*)&lastWrite) &&
		liveCode.LastWrite < lastWrite)
	{
		if (liveCode.Module)
		{
			FreeLibrary((HMODULE)liveCode.Module);
			liveCode.Module = NULL;
			liveCode.Procs.clear();
		}

		std::wstring tempName = liveCode.DllPath + L"_TEMP";
		if (CopyFileW(liveCode.DllPath.c_str(), tempName.c_str(), FALSE))
		{
			liveCode.Module = LoadLibraryW(tempName.c_str());
			if (liveCode.Module)
			{
				for (const auto& proc : liveCode.ProcList)
				{
					liveCode.Procs[proc] = GetProcAddress((HMODULE)liveCode.Module, proc.c_str());
				}
				liveCode.LastWrite = lastWrite;
			}
		}
		std::cout << "Reloaded : " << "LiveCodeFunc" << " from " << liveCode.DllPath.c_str() << "[ " << liveCode.LastWrite << " ]\n";
	}
	CloseHandle(handle);
}
#endif

int main(int argc, char* argv[])
{
#ifdef NDEBUG
	HINSTANCE module = LoadLibraryA("W:\\repos\\LiveReload\\Release\\Core.dll");
	LiveCodePtr LiveCodeFunc = (LiveCodePtr)GetProcAddress(module, "LiveCodeFunc");
	int i = 3;
	while (i--)
	{
		LiveCodeFunc();
		Sleep(500.0);
	}
	FreeLibrary(module);
#else
	std::wstring directory(argv[0], argv[0] + strlen(argv[0]));
	directory.erase(directory.find_last_of(L'\\') + 1);

	LiveCode liveCode;
	liveCode.DllPath = directory + L"Core.dll";
	liveCode.ProcList = { "LiveCodeFunc" };

	while (true)
	{
		Poll(liveCode);

		auto LiveCodePtr = (decltype(LiveCodeFunc)*)liveCode.Procs["LiveCodeFunc"];

		if (LiveCodePtr)
		{
			LiveCodePtr();
		}
		else
		{
			std::cout << "Error" << std::endl;
		}

		Sleep(500.0);
	}
#endif

	return 0;
}
