#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

CommandQueue::~CommandQueue()
{
	CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC cd = {};
	cd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	device->CreateCommandQueue(&cd, IID_PPV_ARGS(&_cmdQueue));
	
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
	
	_cmdList->Close();

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_resCmdList));

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
	// 새 울타리 지점을 만들 때마다 값이 1씩 증가한다.
	++_currentFence;
	// 0 ................. 1	
	// ex) 1까지의 명령들을 실행 한다는 것

	// 새 울타리 지점을 만드는 함수
	_cmdQueue->Signal(_fence.Get(), _currentFence);

	// GPU가 해당 울타리까지의 명령을 완료할 때까지 기다린다.
	if (_fence->GetCompletedValue() < _currentFence)
	{
		// GPU가 현재 울타리 지점에 도달했을 때 발생 할 이벤트를 지정함.
		_fence->SetEventOnCompletion(_currentFence, _fenceEvent);

		// 지정한 객체 ( _fenceEvent ) 가 기다린다. 
		// INFINTY -> 신호가 왔을 때만 함수를 반환
		WaitForSingleObject(_fenceEvent, INFINITE);
	}

}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rc)
{
	_cmdAllocator->Reset();
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);

	// 자원 상태 전이
	// 자원의 사용 방법에 대한 상수
	// 화면 출력 -> 렌더 타겟?
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	_swapChain->GetBackBufferResource().Get(),
		D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물

	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	GEngine->GetCB()->Clear();
	GEngine->GetTableDescHeap()->Clear();

	ID3D12DescriptorHeap* descHeap = GEngine->GetTableDescHeap()->GetDescriptorHeap().Get();
	CMD_LIST->SetDescriptorHeaps(1, &descHeap);

	// 자원 장벽이 명령 목록에 추가됨
	_cmdList->ResourceBarrier(1, &barrier);

	// 뷰포트 배열을 파이프라인의 래스터라이저 단계에 바인딩합니다.
	_cmdList->RSSetViewports(1, vp);
	// Binds an array of scissor rectangles to the rasterizer stage.
	_cmdList->RSSetScissorRects(1, rc);

	// Specify the buffers we are going to render to.
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();
	_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView = GEngine->GetDepthStencilBuffer()->GetDSVCpuHandle();
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &DepthStencilView);
	
	_cmdList->ClearDepthStencilView(DepthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void CommandQueue::RenderEnd()
{	
	// 1. 자원 상태 전이
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackBufferResource().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
		D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	// 2. 자원 장벽이 명령 목록에 추가됨
	_cmdList->ResourceBarrier(1, &barrier);

	_cmdList->Close();

	// 3. 커맨드 리스트 수행
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	// 4. 제출
	_swapChain->Present();

	// gpu 일 끝날 때 까지 대기(?)
	WaitSync();

	// 더블 버퍼링?
	_swapChain->SwapIndex();
}

void CommandQueue::FlushResourceCommandQueue()
{
	_resCmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get()};
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitSync();

	_resCmdAllocator->Reset();
	_resCmdList->Reset(_resCmdAllocator.Get(), nullptr);
}

