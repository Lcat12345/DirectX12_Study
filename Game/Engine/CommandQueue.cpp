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
	// �� ��Ÿ�� ������ ���� ������ ���� 1�� �����Ѵ�.
	++_currentFence;
	// 0 ................. 1	
	// ex) 1������ ��ɵ��� ���� �Ѵٴ� ��

	// �� ��Ÿ�� ������ ����� �Լ�
	_cmdQueue->Signal(_fence.Get(), _currentFence);

	// GPU�� �ش� ��Ÿ�������� ����� �Ϸ��� ������ ��ٸ���.
	if (_fence->GetCompletedValue() < _currentFence)
	{
		// GPU�� ���� ��Ÿ�� ������ �������� �� �߻� �� �̺�Ʈ�� ������.
		_fence->SetEventOnCompletion(_currentFence, _fenceEvent);

		// ������ ��ü ( _fenceEvent ) �� ��ٸ���. 
		// INFINTY -> ��ȣ�� ���� ���� �Լ��� ��ȯ
		WaitForSingleObject(_fenceEvent, INFINITE);
	}

}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rc)
{
	_cmdAllocator->Reset();
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);

	// �ڿ� ���� ����
	// �ڿ��� ��� ����� ���� ���
	// ȭ�� ��� -> ���� Ÿ��?
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	_swapChain->GetBackBufferResource().Get(),
		D3D12_RESOURCE_STATE_PRESENT, // ȭ�� ���
		D3D12_RESOURCE_STATE_RENDER_TARGET); // ���� �����

	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	GEngine->GetCB()->Clear();
	GEngine->GetTableDescHeap()->Clear();

	ID3D12DescriptorHeap* descHeap = GEngine->GetTableDescHeap()->GetDescriptorHeap().Get();
	CMD_LIST->SetDescriptorHeaps(1, &descHeap);

	// �ڿ� �庮�� ��� ��Ͽ� �߰���
	_cmdList->ResourceBarrier(1, &barrier);

	// ����Ʈ �迭�� ������������ �����Ͷ����� �ܰ迡 ���ε��մϴ�.
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
	// 1. �ڿ� ���� ����
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackBufferResource().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // ���� �����
		D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ���

	// 2. �ڿ� �庮�� ��� ��Ͽ� �߰���
	_cmdList->ResourceBarrier(1, &barrier);

	_cmdList->Close();

	// 3. Ŀ�ǵ� ����Ʈ ����
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	// 4. ����
	_swapChain->Present();

	// gpu �� ���� �� ���� ���(?)
	WaitSync();

	// ���� ���۸�?
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

