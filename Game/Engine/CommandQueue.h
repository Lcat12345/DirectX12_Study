#pragma once

// gpu ����� �ѹ��� ��Ƽ� ����
// ������ ����� ��ɵ��� �� ó���Ǳ� ������ ��� ��⿭�� ����� ��ɵ��� �ٷ� ������� �ʴ´�.
// ��� ��⿭�� ��� gpu�� ���.
// �ݸ� ��⿭�� ������ gpu�� ����� ó���� ��⿭�� �ڸ��� ���� �� ���� cpu�� ���.

class SwapChain;
class DescriptorHeap;

class CommandQueue
{
public:
	~CommandQueue();

	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	void WaitSync();		// cpu gpu ����ȭ �۾�
							// �� ��Ÿ�� ������ ����� �۾�
	
	void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rc);
	void RenderEnd();

	void FlushResourceCommandQueue();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return _cmdList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return _resCmdList; }

private:
	// ��Ÿ���� ����
	// GPU�� ��� ��⿭�� ��ɵ� ��
	// Ư�� ���������� �����
	// �� ó���� �� ���� CPU�� ��ٸ��� �ϴ� ��
	ComPtr<ID3D12Fence>		_fence;
	uint64					_currentFence;
	HANDLE					_fenceEvent;

	ComPtr<ID3D12CommandQueue>			_cmdQueue; // ��� ��⿭
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList; // �׸��� ��ɵ��� ��� ��ɸ��

	// ���ҽ� ���ε�� Ŀ�ǵ� ����Ʈ
	ComPtr<ID3D12CommandAllocator>		_resCmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

	shared_ptr<SwapChain>		_swapChain;
};

