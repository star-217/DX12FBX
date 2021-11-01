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
		MAX_SHADER_RESOURCE = 1024 * 10,	//�V�F�[�_�[���\�[�X�̍ő吔�B
		MAX_CONSTANT_BUFFER = 1024 * 10,	//�萔�o�b�t�@�̍ő吔�B
		MAX_SAMPLER_STATE = 16,	//�T���v���X�e�[�g�̍ő吔�B
	};
	int m_numShaderResource = 0;	//�V�F�[�_�[���\�[�X�̐��B
	int m_numConstantBuffer = 0;	//�萔�o�b�t�@�̐��B
	int m_numUavResource = 0;		//�A���I�[�_�[�A�N�Z�X���\�[�X�̐��B
	int m_numSamplerDesc = 0;		//�T���v���̐��B
	ID3D12DescriptorHeap* m_descriptorHeap[2] = { nullptr };					//�f�B�X�N���v�^�q�[�v�B
	//std::vector<FBXIShaderResource*> m_shaderResources;		//�V�F�[�_�[���\�[�X�B
	//std::vector<FBXIUnorderAccessResrouce*> m_uavResoruces;	//UAV���\�[�X�B
	std::vector<FBXConstantBuffer*> m_constantBuffers;		//�萔�o�b�t�@�B
	D3D12_SAMPLER_DESC m_samplerDescs[MAX_SAMPLER_STATE];						//�T���v���X�e�[�g�B
	D3D12_GPU_DESCRIPTOR_HANDLE m_cbGpuDescriptorStart[2];						//�萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h���B
	D3D12_GPU_DESCRIPTOR_HANDLE m_srGpuDescriptorStart[2];						//�V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h���B
	D3D12_GPU_DESCRIPTOR_HANDLE m_uavGpuDescriptorStart[2];						//UAV���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h���B
	D3D12_GPU_DESCRIPTOR_HANDLE m_samplerGpuDescriptorStart[2];
};
