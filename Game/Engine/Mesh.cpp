#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"

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
	// 1. ���ۿ� ������ ����
	// 2. TablerDescriptorHeap�� CBV ����
	// 3. ��� ������ CommitTable ȣ��

	CONSTANT_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&_transform, sizeof(_transform));

	_material->Update();

	GEngine->GetTableDescHeap()->CommitTable();

	// CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
	CMD_LIST->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// ������� �ϸ� HEAP_TYPE_DEFAULT���� 2���� ����� �־���
	// �����͸� ���� �� ���� HEAP_TYPE_UPLOAD�� �̿�.

	// � �뵵�� ���۸� Ȱ���� ������ properties�� ä��
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// vertexbuffer : gpu ��򰡿� ������� ������ ����Ű�� ������
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer)
	);

	// vertexDataBuffer : gpu�� �ӽ����� ������ ����� �����͸� �����Ѵ�.
	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	/*
	�Ű�����:
	Subresource: ������ ���긮�ҽ��� �ε����� �����մϴ�. ��κ��� ���ҽ��� ���, �� ���� 0���� �����ϸ� �˴ϴ�.
	ReadRange: ���ҽ����� ���� ����Ʈ ������ �����մϴ�. ��ü �б� �׼����� ���� �̸� nullptr�� �����մϴ�.
	WriteRange: ���ҽ��� �� ����Ʈ ������ �����մϴ�. ��ü ���� �׼����� ���� �̸� nullptr�� �����մϴ�.
	ppData: ���ε� �������� �ּҸ� ���� ������ ������ �����մϴ�.
	*/
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	// �ӽ����� ������ �����͸� ������
	memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	// �Ѳ��� ����(?)
	_vertexBuffer->Unmap(0, nullptr);

	// view�� �����.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // ���� 1���� ũ��
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