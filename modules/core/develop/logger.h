#ifndef __DEVELOP_LOGGER_H__
#define __DEVELOP_LOGGER_H__

namespace develop
{
	void DebugPrint(const wchar_t* format, ...);
	void LogAssert(const wchar_t* msg);
}
#endif // __DEBUGUTILES_H__