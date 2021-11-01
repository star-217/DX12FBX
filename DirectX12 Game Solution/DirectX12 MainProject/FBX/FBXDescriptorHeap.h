#pragma once

class FBXDescriptorHeap {
public:
	FBXDescriptorHeap();
	~FBXDescriptorHeap();

	ID3D12DescriptorHeap* Get() const;

	void Commit();

	void CommitSamperHeap();

private:
	enum {
		MAX_SHADER_RESOURCE = 1024 * 10,	//シェーダーリソースの最大数。
		MAX_CONSTANT_BUFFER = 1024 * 10,	//定数バッファの最大数。
		MAX_SAMPLER_STATE = 16,	//サンプラステートの最大数。
	};
	int m_numShaderResource = 0;	//シェーダーリソースの数。
	int m_numConstantBuffer = 0;	//定数バッファの数。
	int m_numUavResource = 0;		//アンオーダーアクセスリソースの数。
	int m_numSamplerDesc = 0;		//サンプラの数。
	ID3D12DescriptorHeap* m_descriptorHeap[2] = { nullptr };					//ディスクリプタヒープ。
	//std::vector<FBXIShaderResource*> m_shaderResources;		//シェーダーリソース。
	//std::vector<FBXIUnorderAccessResrouce*> m_uavResoruces;	//UAVリソース。
	std::vector<FBXConstantBuffer*> m_constantBuffers;		//定数バッファ。
	D3D12_SAMPLER_DESC m_samplerDescs[MAX_SAMPLER_STATE];						//サンプラステート。
	D3D12_GPU_DESCRIPTOR_HANDLE m_cbGpuDescriptorStart[2];						//定数バッファのディスクリプタヒープの開始ハンドル。
	D3D12_GPU_DESCRIPTOR_HANDLE m_srGpuDescriptorStart[2];						//シェーダーリソースのディスクリプタヒープの開始ハンドル。
	D3D12_GPU_DESCRIPTOR_HANDLE m_uavGpuDescriptorStart[2];						//UAVリソースのディスクリプタヒープの開始ハンドル。
	D3D12_GPU_DESCRIPTOR_HANDLE m_samplerGpuDescriptorStart[2];
};
