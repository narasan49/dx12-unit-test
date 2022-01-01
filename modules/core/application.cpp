#include "pch.h"
#include "Application.h"
#include "DX12Device.h"

namespace module
{
	LRESULT windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_DESTROY)
		{
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	Application* Application::mpInstance = nullptr;

	Application::Application() :
		mpWindowWidth(1280), mpWindowHeight(720),
		mpWndClass({}), mpDevice(nullptr), mHwnd(NULL)
	{

	}

	void Application::createWindow()
	{
		mpWndClass.cbSize = sizeof(WNDCLASSEX);
		mpWndClass.lpfnWndProc = windowProcedure;
		mpWndClass.lpszClassName = L"DX12WithTest";
		mpWndClass.hInstance = GetModuleHandle(nullptr);

		RegisterClassEx(&mpWndClass);
		RECT rect = { 0, 0, mpWindowWidth, mpWindowHeight };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

		mHwnd = CreateWindow(
			mpWndClass.lpszClassName,
			L"DX12WithTest",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			mpWndClass.hInstance,
			nullptr);

		ShowWindow(mHwnd, SW_SHOW);
	}

	void Application::initialize()
	{
		createWindow();

		mpDevice = new DX12Device();
		mpDevice->createDevice();
		mpDevice->initializeCommand(0, D3D12_COMMAND_LIST_TYPE_DIRECT);
		mpDevice->createSwapChain(mHwnd, mpWindowWidth, mpWindowHeight);
		mpDevice->createRTV();
	}

	void Application::run()
	{
		MSG msg = {};

		while (true)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
			{
				break;
			}

			mpDevice->getCmdAlloc()->Reset();
			auto swapChain = mpDevice->getSwapChain();
			auto backBufIdx = swapChain->GetCurrentBackBufferIndex();
			auto cmdList = mpDevice->getCmdList();
			auto hRtv = mpDevice->getRtvHeap()->GetCPUDescriptorHandleForHeapStart();
			hRtv.ptr += backBufIdx * static_cast<size_t>(mpDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
			cmdList->OMSetRenderTargets(1, &hRtv, true, nullptr);

			cmdList->ClearRenderTargetView(hRtv, mClearColor, 0, nullptr);
			cmdList->Close();

			mpDevice->executeCommands();
			swapChain->Present(1, 0);

		}
	}

	void Application::finalize()
	{
		UnregisterClass(mpWndClass.lpszClassName, mpWndClass.hInstance);
	}
}