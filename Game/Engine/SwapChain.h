#pragma once

// 외주를 주고 받음
// 텍스처 버퍼 2개를 사용 ( 전면, 후면 )
// 후면 버퍼에 다음 프레임에 나올 픽셀들을 그리고
// 다음 프레임 때 전면 버퍼와 후면 버퍼의 포인터를 바꿈 ( 더블 버퍼링 )


class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();		// 제출
	void SwapIndex();	// 후면 버퍼와 전면 버퍼를 교체한다.

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _rtvBuffer[index]; }

	ComPtr<ID3D12Resource> GetBackBufferResource() { return _rtvBuffer[_backBufferIndex]; }		

	D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return _rtvHandle[_backBufferIndex]; }

private:
	void CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void CreateRTV(ComPtr<ID3D12Device> device);

private:
	ComPtr<IDXGISwapChain>		 _swapChain;
	
	ComPtr<ID3D12Resource>		 _rtvBuffer[SWAP_CHAIN_BUFFER_COUNT];		// 그려지는 종이
	ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE	 _rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

	uint32						 _backBufferIndex = 0;


};

