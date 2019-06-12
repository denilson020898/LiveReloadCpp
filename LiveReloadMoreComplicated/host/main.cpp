#include "platform.h"
#include <iostream>
#include <Windows.h>

typedef int(*pFunc)(int);

static const char *kDllName = "mydll.dll";
static const char *kDllTempName = "mydll_temp.dll";
static const char *kSymbolName = "addTwo";

enum LibraryStatus
{
	LibraryStatus_Failure = INT_MIN,
	LibraryStatus_InvalidLibrary,
	LibraryStatus_InvalidSymbol,
	LibraryStatus_InvalidHandle,
	LibraryStatus_UnloadFailure,
	LibraryStatus_Success = 0
};

struct DLLCode
{
	DLLHandle handle;
	FileTime lastModified;

	pFunc func;
	bool isValid;
};


inline DLLHandle loadSharedLibrary(const char*filename)
{
	DLLHandle libHandle = LoadLibraryA(filename);
	if (!libHandle)
	{
		OSPrintLastError();
		return NULL;
	}
	//std::cout << "loaded a library" << std::endl;
	return libHandle;
}

inline int unloadSharedLibrary(DLLHandle handle)
{
	if (!handle)
	{
		perror("The handle is not valid! Nothing to unload!\n");
		return -1;
	}
	BOOL result = FreeLibrary(handle);
	if (result == 0)
	{
		OSPrintLastError();
		return -2;
	}
	//std::cout << "unloaded a library" << std::endl;
	return 0;
}

inline FuncPtr loadSymbolFromLibrary(DLLHandle handle, const char *symbol)
{
	FuncPtr symbolAddr = GetProcAddress(handle, symbol);
	if (!symbolAddr)
	{
		OSPrintLastError();
	}
	//std::cout << "loaded a symbol" << std::endl;
	return symbolAddr;
}

static DLLCode kDLLCode = {};
LibraryStatus loadLogicDLL(DLLCode &library)
{
	CopyFile(kDllName, kDllTempName, FALSE);
	const char *libFileNameC = kDllTempName;

	FileTime lastModified = getLastWriteTime(libFileNameC);
	library.lastModified = lastModified;

	DLLHandle handle = loadSharedLibrary(libFileNameC);
	if (!handle)
	{
		std::cout << "unable to load a dll library" << std::endl;
		library.handle = NULL;
		library.isValid = false;

		return LibraryStatus_InvalidLibrary;
	}

	library.handle = handle;
	FuncPtr funcSymbol = loadSymbolFromLibrary(handle, kSymbolName);
	if (!funcSymbol)
	{
		std::cout << "unable to load a symbol" << std::endl;
		return LibraryStatus_InvalidSymbol;
	}
	library.func = (pFunc)funcSymbol;
	library.isValid = true;
	std::cout << "loaded library from: " << kDllTempName << std::endl;
	return LibraryStatus_Success;
}

LibraryStatus unloadLogicDLL(DLLCode &library)
{
	if (!kDLLCode.isValid)
	{
		return LibraryStatus_InvalidHandle;
	}
	int unload = unloadSharedLibrary(kDLLCode.handle);
	if (unload != 0)
	{
		std::cout << "unable to unload a dll" << std::endl;
		return LibraryStatus_UnloadFailure;
	}
	library.func = NULL;
	library.lastModified = {};
	library.isValid = false;
	//DeleteFile(kDllTempName);
	return LibraryStatus_Success;
}


int main()
{

	LibraryStatus result = loadLogicDLL(kDLLCode);
	if (result != LibraryStatus_Success)
	{
		std::cout << "Failed to load a dll" << std::endl;
	}

	while (true)
	{
		LibraryStatus status;

		if (!kDLLCode.isValid)
		{
			unloadLogicDLL(kDLLCode);
			status = loadLogicDLL(kDLLCode);
			if (status != LibraryStatus_Success)
			{
				return -1;
			}
		}

		FileTime lastModified = getLastWriteTime(kDllName);
		std::cout << std::endl;
		std::cout << "original DLL :" << lastModified << std::endl;
		std::cout << "copied dll   :" << kDLLCode.lastModified << std::endl;

		if (lastModified >= 0 && lastModified != kDLLCode.lastModified)
		{
			std::cout << "New DLL Detected!" << std::endl;
			status = unloadLogicDLL(kDLLCode);
			if (status != LibraryStatus_Success)
			{ 
			std::cout << "unloaded the old dll with time " << lastModified << std::endl;
			return -1;
			}

			status = loadLogicDLL(kDLLCode);
			if (status != LibraryStatus_Success)
			{
				std::cout << "Failed to load a dll" << std::endl;
			}
		}

		int num = kDLLCode.func(2);
		std::cout << "The dll said: " << num << std::endl;
		Sleep(1000);
	}
	
	return 0;
}

// some changes