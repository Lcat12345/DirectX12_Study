#pragma once

// ���ָ� �ְ� ����
// �ؽ�ó ���� 2���� ��� ( ����, �ĸ� )
// �ĸ� ���ۿ� ���� �����ӿ� ���� �ȼ����� �׸���
// ���� ������ �� ���� ���ۿ� �ĸ� ������ �����͸� �ٲ� ( ���� ���۸� )


class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();		// ����
	void SwapIndex();	// �ĸ� ���ۿ� ���� ���۸� ��ü�Ѵ�.

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _renderTargets[index]; }

	uint32 GetBackBufferIndex() { return _backBufferIndex; }
	ComPtr<ID3D12Resource> GetBackBufferResource() { return _renderTargets[_backBufferIndex]; }

private:
	ComPtr<ID3D12Resource>	_renderTargets[SWAP_CHAIN_BUFFER_COUNT];	// �׷����� ����
	ComPtr<IDXGISwapChain>	_swapChain;
	uint32					_backBufferIndex = 0;


};

