#include "pch.h"
using namespace module;
TEST(TestDX12Device, CreateDevice)
{
	auto app = Application::instance();
	app->createWindow();
	auto hwnd = app->getHwnd();

	DX12Device device;
	EXPECT_TRUE(device.createDevice());
	EXPECT_HRESULT_SUCCEEDED(device.initializeCommand(0, D3D12_COMMAND_LIST_TYPE_DIRECT));
	EXPECT_HRESULT_SUCCEEDED(device.createSwapChain(hwnd, app->getWindowWidth(), app->getWindowHeight()));
	EXPECT_HRESULT_SUCCEEDED(device.createRTV());
}