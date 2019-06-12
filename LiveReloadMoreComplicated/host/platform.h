#ifndef PLATFORM_H
#define PLATFORM_H
	
#include <Windows.h>
#define Shared extern "C"	
#define DLLExport __declspec(dllexport)	
typedef HMODULE DLLHandle;
typedef FARPROC FuncPtr;
	
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/// Aliases for basic size types
typedef short int sint;
typedef unsigned short int usint;

typedef unsigned int uint;

typedef uint8_t uint8;
typedef uint8 u8;
typedef int8_t int8;
typedef int8 i8;

typedef uint16_t uint16;
typedef uint16 u16;
typedef int16_t int16;
typedef int16 i16;

typedef uint32_t uint32;
typedef uint32 u32;
typedef int32_t int32;
typedef int32 i32;

typedef uint64_t uint64;
typedef uint64 u64;
typedef int64_t int64;
typedef int64 i64;

typedef size_t sizet;

typedef time_t timet;

typedef long long dlong;
typedef unsigned long long udlong;

typedef u64 FileTime;

/// stream
inline void OSPrintLastError()
{
	char errMsg[256];
	DWORD errCode = GetLastError();
	if (errCode == 0) {
		return;
	}
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)errMsg,
		sizeof(errMsg),
		NULL);

	perror(errMsg);
}

/// lastwritetime
inline FileTime getLastWriteTime(const char *filename)
{
	FileTime result = -1;
	FILETIME lastWriteTime;
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (GetFileAttributesEx((LPCTSTR)filename, GetFileExInfoStandard, &data))
	{
		lastWriteTime = data.ftLastWriteTime;
	}
	else
	{
		OSPrintLastError();
		return result;
	}

	result = (FileTime)lastWriteTime.dwHighDateTime << sizeof(DWORD) | lastWriteTime.dwLowDateTime;

	return result;
}

#endif // PLATFORM_H