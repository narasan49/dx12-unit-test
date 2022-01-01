#ifndef MODULE_DX12DEVICE_H
#define MODULE_DX12DEVICE_H
using namespace Microsoft::WRL;
using namespace std;

namespace module
{
	class DX12Device
	{
	public:
		DX12Device();

		bool createDevice();
		HRESULT initializeCommand(UINT nodeMask, D3D12_COMMAND_LIST_TYPE type);
		HRESULT createSwapChain(const HWND& hwnd, UINT width, UINT height);
		HRESULT createRTV();

		void executeCommands();

		void enableDebugLayer();

		ComPtr<ID3D12Device> getDevice() const { return mpDev; }
		ComPtr<IDXGISwapChain4> getSwapChain() const { return mpSwapChain; }
		ComPtr<ID3D12CommandAllocator> getCmdAlloc() const { return mpCmdAllocator; }
		ComPtr<ID3D12GraphicsCommandList> getCmdList() const { return mpCmdList; }
		ComPtr<ID3D12DescriptorHeap> getRtvHeap() const { return mpRtvHeap; }

	private:
		ComPtr<ID3D12Device> mpDev;
		ComPtr<IDXGIFactory6> mpDxgiFactory;
		ComPtr<IDXGISwapChain4> mpSwapChain;

		ComPtr<ID3D12CommandAllocator> mpCmdAllocator;
		ComPtr<ID3D12GraphicsCommandList> mpCmdList;
		ComPtr<ID3D12CommandQueue> mpCmdQueue;

		ComPtr<ID3D12DescriptorHeap> mpRtvHeap;
		vector<ComPtr<ID3D12Resource>> mpBackBuffers;
	};
}

#endif // !MODULE_DX12DEVICE_H