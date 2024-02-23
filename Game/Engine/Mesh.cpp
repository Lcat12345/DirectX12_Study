#include "pch.h"
#include "Mesh.h"

#include "Engine.h"

void Mesh::Init(const vector<Vertex>& vertex, const vector<uint32>& index)
{
	CreateVertexBuffer(vertex);
	CreateIndexBuffer(index);
}

void Mesh::Render()
{
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView);
	CMD_LIST->IASetIndexBuffer(&_indexBufferView);
	
	// TODO
	// 1. 버퍼에 데이터 세팅
	// 2. TablerDescriptorHeap에 CBV 전달
	// 3. 모두 끝나면 CommitTable 호출
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle = GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
		GEngine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b0);

		GEngine->GetTableDescHeap()->SetSRV(_tex->GetCpuHandle(), SRV_REGISTER::t0);
	}


	GEngine->GetTableDescHeap()->CommitTable();

	// CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
	CMD_LIST->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// 정석대로 하면 HEAP_TYPE_DEFAULT까지 2개를 만들어 둬야함
	// 데이터를 전송 할 때만 HEAP_TYPE_UPLOAD를 이용.

	// 어떤 용도로 버퍼를 활용할 것인지 properties를 채움
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// vertexbuffer : gpu 어딘가에 만들어진 공간을 가리키는 포인터
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer)
	);

	// vertexDataBuffer : gpu에 임시적인 공간을 만들고 데이터를 복사한다.
	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	/*
	매개변수:
	Subresource: 매핑할 서브리소스의 인덱스를 지정합니다. 대부분의 리소스의 경우, 이 값을 0으로 설정하면 됩니다.
	ReadRange: 리소스에서 읽을 바이트 범위를 지정합니다. 전체 읽기 액세스를 위해 이를 nullptr로 설정합니다.
	WriteRange: 리소스에 쓸 바이트 범위를 지정합니다. 전체 쓰기 액세스를 위해 이를 nullptr로 설정합니다.
	ppData: 매핑된 데이터의 주소를 받을 포인터 변수를 지정합니다.
	*/
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	// 임시적인 공간에 데이터를 복사함
	memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	// 뚜껑을 닫음(?)
	_vertexBuffer->Unmap(0, nullptr);

	// view를 만든다.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개의 크기
	_vertexBufferView.SizeInBytes = bufferSize;
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	_indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_indexBuffer)
	);

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);

	_indexBuffer->Map(0, &readRange, &indexDataBuffer);
	memcpy(indexDataBuffer, &buffer[0], bufferSize);
	_indexBuffer->Unmap(0, nullptr);

	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = bufferSize;
}