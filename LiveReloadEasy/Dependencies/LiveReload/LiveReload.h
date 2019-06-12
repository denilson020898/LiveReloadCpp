#pragma once

#ifdef LIVE_CODE_EXPORT
	#define LIVE_CODE_API __declspec(dllexport)
#else
	#define LIVE_CODE_API __declspec(dllimport)
#endif

