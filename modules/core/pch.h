// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef MODULE_CORE_PCH_H
#define MODULE_CORE_PCH_H

// add headers that you want to pre-compile here
#include <Windows.h>
#include <iostream>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <assert.h>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include "develop/Logger.h"
#include "Application.h"

#endif //MODULE_CORE_PCH_H
