#include "pch.h"
#include "DX12Device.h"

using namespace std;

namespace
{
	D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	};
}

namespace module
{
	DX12Device::DX12Device()
	{
#if _DEBUG
		enableDebugLayer();
#endif // _DEBUG
	}

	bool DX12Device::createDevice()
	{
#if _DEBUG
		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(mpDxgiFactory.ReleaseAndGetAddressOf()));
#else
		auto result = CreateDXGIFactory1(IID_PPV_ARGS(mpDxgiFactory.ReleaseAndGetAddressOf()));
#endif // _DEBUG

		if (FAILED(result))
		{
			develop::LogAssert(L"DXGIファクトリーの作成に失敗しました。");
			return false;
		}

		vector<IDXGIAdapter*> pAdapters;
		IDXGIAdapter* pTmpAdapter = nullptr;
		for (UINT i = 0; mpDxgiFactory->EnumAdapters(i, &pTmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
		{
			pAdapters.push_back(pTmpAdapter);
		}

		for (auto pAdapter : pAdapters)
		{
			DXGI_ADAPTER_DESC desc = {};
			pAdapter->GetDesc(&desc);
			wstring descStr = desc.Description;
			develop::DebugPrint(descStr.c_str());
			if (descStr.find(L"NVIDIA") != string::npos)
			{
				pTmpAdapter = pAdapter;
				break;
			}
		}

		for (auto level : levels)
		{
			if (SUCCEEDED(D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(mpDev.ReleaseAndGetAddressOf()))))
			{
				break;
			}
		}

		if (!mpDev.Get())
		{
			develop::LogAssert(L"DX12デバイスの作成に失敗しました");
		}
		return mpDev.Get() != nullptr;
	}

	HRESULT DX12Device::initializeCommand(UINT nodeMask, D3D12_COMMAND_LIST_TYPE type)
	{
		auto result = mpDev->CreateCommandAllocator(type, IID_PPV_ARGS(mpCmdAllocator.ReleaseAndGetAddressOf()));
		if (FAILED(result))
		{
			develop::LogAssert(L"コマンドアロケータの作成に失敗しました。");
			return result;
		}

		result = mpDev->CreateCommandList(nodeMask, type, mpCmdAllocator.Get(), nullptr, IID_PPV_ARGS(mpCmdList.ReleaseAndGetAddressOf()));;

		assert(result == S_OK && L"コマンドリストの作成に失敗しました。");
		if (FAILED(result))
		{
			develop::LogAssert(L"コマンドリストの作成に失敗しました。");
			return result;
		}

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = nodeMask;

		result = mpDev->CreateCommandQueue(&desc, IID_PPV_ARGS(mpCmdQueue.ReleaseAndGetAddressOf()));
		if (FAILED(result))
		{
			develop::LogAssert(L"コマンドキューの作成に失敗しました。");
		}
		return result;
	}

	HRESULT DX12Device::createSwapChain(const HWND& hwnd, UINT width, UINT height)
	{
		// https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/ns-dxgi1_2-dxgi_swap_chain_desc1
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		desc.BufferCount = 2;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		auto result = mpDxgiFactory->CreateSwapChainForHwnd(
			mpCmdQueue.Get(),
			hwnd,
			&desc,
			nullptr,
			nullptr,
			reinterpret_cast<IDXGISwapChain1**>(mpSwapChain.ReleaseAndGetAddressOf()));

		if (FAILED(result))
		{
			develop::LogAssert(L"スワップチェーンの作成に失敗しました。");
			mpSwapChain = nullptr;
		}
		return result;
	}

	HRESULT DX12Device::createRTV()
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = 2;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		auto result = mpDev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mpRtvHeap.ReleaseAndGetAddressOf()));
		if (FAILED(result))
		{
			develop::LogAssert(L"RTVヒープの作成に失敗しました。");
			return result;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		result = mpSwapChain->GetDesc(&swapChainDesc);

		mpBackBuffers.resize(swapChainDesc.BufferCount);
		auto handle = mpRtvHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT idx = 0; idx < swapChainDesc.BufferCount; idx++)
		{
			result = mpSwapChain->GetBuffer(idx, IID_PPV_ARGS(mpBackBuffers[idx].ReleaseAndGetAddressOf()));
			if (FAILED(result))
			{
				develop::LogAssert(L"バックバッファの取得に失敗しました。");
				return result;
			}
			mpDev->CreateRenderTargetView(mpBackBuffers[idx].Get(), nullptr, handle);
			handle.ptr += mpDev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		return S_OK;
	}

	void DX12Device::executeCommands()
	{
		ID3D12CommandList* cmdLists[] = { mpCmdList.Get() };
		mpCmdQueue->ExecuteCommandLists(1, cmdLists);
		mpCmdAllocator->Reset();
		mpCmdList->Reset(mpCmdAllocator.Get(), nullptr);
	}

	void DX12Device::enableDebugLayer()
	{
		ID3D12Debug* debugLayer = nullptr;
		auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}