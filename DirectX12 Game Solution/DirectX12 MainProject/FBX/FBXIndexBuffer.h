#pragma once

class FBXIndexBuffer
{
public:
	~FBXIndexBuffer();

	void Init(int size,int stride);

	void Copy(uint16_t* srcIndecies);

	void Copy(uint32_t* srcIndecies);

private:
	ID3D12Resource* m_indexBuffer = nullptr;	//�C���f�b�N�X�o�b�t�@�B
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;	//�C���f�b�N�X�o�b�t�@�r���[�B
	int m_count = 0;							//�C���f�b�N�X�̐��B
	int m_strideInBytes = 0;					//�X�g���C�h(�P�ʁF�o�C�g)�B
	int m_sizeInBytes = 0;						//�T�C�Y(�P�ʁF�o�C�g)�B
};