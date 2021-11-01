#include "FBXMain.h"
#include "FBXIndexBuffer.h"

FBXIndexBuffer::~FBXIndexBuffer()
{
	if (m_indexBuffer)
		m_indexBuffer->Release();
}

void FBXIndexBuffer::Init(int size, int stride)
{
	if (stride == 2){
		m_sizeInBytes = size * 2;
	}
	else{
		m_sizeInBytes = size;
	}
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(m_sizeInBytes);
	auto re = DXTK->Device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_indexBuffer)
	);

	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();

	m_strideInBytes = 4;
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = m_sizeInBytes;

	m_count = m_sizeInBytes / m_strideInBytes;
}

void FBXIndexBuffer::Copy(uint16_t* srcIndecies)
{
	uint32_t* pData;
	m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	for (int i = 0; i < m_count; i++) {
		pData[i] = srcIndecies[i];
	}
	m_indexBuffer->Unmap(0, nullptr);
}

void FBXIndexBuffer::Copy(uint32_t* srcIndecies)
{
	uint32_t* pData;
	m_indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	for (int i = 0; i < m_count; i++) {
		pData[i] = srcIndecies[i];
	}
	m_indexBuffer->Unmap(0, nullptr);
}
