#include "pch.h"
#include "Mesh.h"

#include "Engine.h"

void Mesh::Init(vector<Vertex>& vec)
{
	_vertexCount = static_cast<uint32>(vec.size());
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
	memcpy(vertexDataBuffer, &vec[0], bufferSize);
	// �Ѳ��� ����(?)
	_vertexBuffer->Unmap(0, nullptr);
	
	// view�� �����.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // ���� 1���� ũ��
	_vertexBufferView.SizeInBytes = bufferSize;
}

void Mesh::Render()
{
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView);
	
	// TODO
	// 1. ���ۿ� ������ ����
	// 2. ������ �ּҸ� register�� ����
	GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
	GEngine->GetCB()->PushData(1, &_transform, sizeof(_transform));
	GEngine->GetCB()->PushData(2, &_transform, sizeof(_transform));
	// CMD_LIST->SetGraphicsRootConstantBufferView(0, ???);

	CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
}
