#pragma once

class FBXIndexBuffer
{
public:
	~FBXIndexBuffer();

	void Init(int size,int stride);

	void Copy(uint16_t* srcIndecies);

	void Copy(uint32_t* srcIndecies);

private:
	ID3D12Resource* m_indexBuffer = nullptr;	//インデックスバッファ。
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;	//インデックスバッファビュー。
	int m_count = 0;							//インデックスの数。
	int m_strideInBytes = 0;					//ストライド(単位：バイト)。
	int m_sizeInBytes = 0;						//サイズ(単位：バイト)。
};