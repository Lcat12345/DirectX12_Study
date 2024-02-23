#pragma once

// gpu 명령을 한번에 모아서 실행
// 이전에 제출된 명령들이 다 처리되기 전까진 명령 대기열에 제출된 명령들은 바로 실행되지 않는다.
// 명령 대기열이 비면 gpu는 논다.
// 반면 대기열이 꽉차면 gpu가 명령을 처리해 대기열에 자리가 생길 때 까지 cpu가 논다.

class SwapChain;
class DescriptorHeap;

class CommandQueue
{
public:
	~CommandQueue();

	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	void WaitSync();		// cpu gpu 동기화 작업
							// 새 울타리 지점을 만드는 작업
	
	void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rc);
	void RenderEnd();

	void FlushResourceCommandQueue();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return _cmdList; }
	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return _resCmdList; }

private:
	// 울타리의 역할
	// GPU가 명령 대기열의 명령들 중
	// 특정 지점까지의 명령을
	// 다 처리할 때 까지 CPU를 기다리게 하는 것
	ComPtr<ID3D12Fence>		_fence;
	uint64					_currentFence;
	HANDLE					_fenceEvent;

	ComPtr<ID3D12CommandQueue>			_cmdQueue; // 명령 대기열
	ComPtr<ID3D12CommandAllocator>		_cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList; // 그리기 명령들이 담긴 명령목록

	// 리소스 업로드용 커맨드 리스트
	ComPtr<ID3D12CommandAllocator>		_resCmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_resCmdList;

	shared_ptr<SwapChain>		_swapChain;
};

