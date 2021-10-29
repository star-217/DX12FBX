#pragma once

class FBXConstantBuffer{
public:
	FBXConstantBuffer() {}

	~FBXConstantBuffer();

	FBXConstantBuffer(FBXConstantBuffer&& r)
	{
		m_constantBuffer[0] = r.m_constantBuffer[0];
		m_constantBuffer[1] = r.m_constantBuffer[1];
		m_constBufferCPU[0] = r.m_constBufferCPU[0];
		m_constBufferCPU[1] = r.m_constBufferCPU[1];
		m_size = r.m_size;
		m_allocSize = r.m_allocSize;
		m_isValid = r.m_isValid;

		r.m_constantBuffer[0] = nullptr;
		r.m_constantBuffer[1] = nullptr;
		r.m_constBufferCPU[0] = nullptr;
		r.m_constBufferCPU[1] = nullptr;
	}

	void Init(int size, void* srcData = nullptr);

	void CopyToVRAM(void* data);
	template< class T>
	void CopyToVRAM(T& data)
	{
		CopyToVRAM(&data);
	}

	void RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);
	void RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo);

private:
	ID3D12Resource* m_constantBuffer[2] = { nullptr };//�萔�o�b�t�@�B
	void* m_constBufferCPU[2] = { nullptr };		//CPU������A�N�Z�X�ł��邷��萔�o�b�t�@�̃A�h���X�B
	int m_size = 0;									//�萔�o�b�t�@�̃T�C�Y�B
	int m_allocSize = 0;
	bool m_isValid = false;
};