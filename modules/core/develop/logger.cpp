#include "../pch.h"
#include "logger.h"

namespace develop
{
	void DebugPrint(const wchar_t* format, ...)
	{
		va_list valist;
		va_start(valist, format);
		std::wstring str(format);
		vwprintf((str + L"\n").c_str(), valist);
		va_end(valist);
	}

	void LogAssert(const wchar_t* msg)
	{
		assert(false && msg);
		DebugPrint(msg);
	}
}