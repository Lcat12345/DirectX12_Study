#pragma once

// 정점으로 이루어진 물체

class Material;

class Mesh
{
public:
	void Init(const vector<Vertex>& vertex, const vector<uint32>& index);
	void Render();

	void SetTransform(const Transform& t) { _transform = t; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

private:
	void CreateVertexBuffer(const vector<Vertex>& buffer);
	void CreateIndexBuffer(const vector<uint32>& buffer);

private:
	ComPtr<ID3D12Resource>		_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32						_vertexCount = 0;

	ComPtr<ID3D12Resource>		_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};
	uint32						_indexCount = 0;

	Transform _transform = {};
	shared_ptr<Material> _material = {};
};

