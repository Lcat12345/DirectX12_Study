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
	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain, shared_ptr<DescriptorHeap> descHeap);

	void WaitSync();		// cpu gpu ����ȭ �۾�
							// �� ��Ÿ�� ������ ����� �۾�
	
	void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rc);
	void RenderEnd();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }


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

	shared_ptr<SwapChain>		_swapChain;
	shared_ptr<DescriptorHeap>	_descHeap;
};

