#include "FBXMain.h"
#include "FBXRootSiginature.h"

enum {
	enDescriptorHeap_CB,
	enDescriptorHeap_SRV,
	enDescriptorHeap_UAV,
	enNumDescriptorHeap
};

bool FBXRootSignature::Init(D3D12_FILTER samplerFilter, D3D12_TEXTURE_ADDRESS_MODE textureAdressModeU, D3D12_TEXTURE_ADDRESS_MODE textureAdressModeV, D3D12_TEXTURE_ADDRESS_MODE textureAdressModeW, UINT maxCbvDescriptor, UINT maxSrvDescriptor, UINT maxUavDescritor)
{
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = samplerFilter;
	sampler.AddressU = textureAdressModeU;
	sampler.AddressV = textureAdressModeV;
	sampler.AddressW = textureAdressModeW;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	return Init(&sampler, 1, maxCbvDescriptor, maxSrvDescriptor, maxUavDescritor);
}

bool FBXRootSignature::Init(D3D12_STATIC_SAMPLER_DESC* samplerDescArray, int numSampler, UINT maxCbvDescriptor, UINT maxSrvDescriptor, UINT maxUavDescritor, UINT offsetInDescriptorsFromTableStartCB, UINT offsetInDescriptorsFromTableStartSRV, UINT offsetInDescriptorsFromTableStartUAV)
{

	CD3DX12_DESCRIPTOR_RANGE1 ranges[enNumDescriptorHeap];
	CD3DX12_ROOT_PARAMETER1 rootParameters[enNumDescriptorHeap];

	ranges[enDescriptorHeap_CB].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, maxCbvDescriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offsetInDescriptorsFromTableStartCB);
	rootParameters[enDescriptorHeap_CB].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_CB]);

	ranges[enDescriptorHeap_SRV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, maxSrvDescriptor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offsetInDescriptorsFromTableStartSRV);
	rootParameters[enDescriptorHeap_SRV].InitAsDescriptorTable(1, &ranges[enDescriptorHeap_SRV]);

	ranges[enDescriptorHeap_UAV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, maxUavDescritor, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, offsetInDescriptorsFromTableStartUAV);

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, numSampler, samplerDescArray, rootSignatureFlags);
	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	auto re = DXTK->Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (FAILED(re)) {
		return false;
	}
	return true;

	return false;
}
