#include "pch.h"
#include "RootSignature.h"

void RootSignature::Init(ComPtr<ID3D12Device> device)
{
	CD3DX12_ROOT_PARAMETER param[3];
	param[0].InitAsConstantBufferView(0);	// 0번 -> b0 -> CBV
	param[1].InitAsConstantBufferView(1);	// 1번 -> b1 -> CBV
	param[2].InitAsConstantBufferView(2);	// 2번 -> b3 -> CBV


	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(3, param);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계
	
	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	// Serialize : 직렬화
	// blobSignatrue : 직렬화된 루트 서명에 액세스하는 데 사용
	// sigDesc를 바이트 형태로 전달하는 것임
	D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));

}
