#pragma once

class FBXRootSignature {
public:
	~FBXRootSignature()
	{
		if (m_rootSignature)
			m_rootSignature->Release();
	}

	bool Init(
		D3D12_FILTER samplerFilter,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeU,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeV,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeW,
		UINT maxCbvDescriptor = 8,
		UINT maxSrvDescriptor = 32,
		UINT maxUavDescritor = 8
	);

	bool Init(
		D3D12_STATIC_SAMPLER_DESC* samplerDescArray,
		int numSampler,
		UINT maxCbvDescriptor = 8,
		UINT maxSrvDescriptor = 32,
		UINT maxUavDescritor = 8,
		UINT offsetInDescriptorsFromTableStartCB = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		UINT offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		UINT offsetInDescriptorsFromTableStartUAV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
	);

	ID3D12RootSignature* Get()
	{
		return m_rootSignature;
	}
private:
	ID3D12RootSignature* m_rootSignature = nullptr;
};