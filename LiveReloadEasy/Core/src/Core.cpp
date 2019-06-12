#include "Core.h"

#include <iostream>

extern "C" LIVE_CODE_API void LiveCodeFunc()
{
	std::cout << "Okay!" << std::endl;
}
