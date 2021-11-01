#include "FBXMain.h"
#include "FBXVertexBuffer.h"

FBXVertexBuffer::~FBXVertexBuffer()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();
}

void FBXVertexBuffer::Init(int size,int stride)
{
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	auto re = DXTK->Device->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer)
	);
	m_vertexBuffer->SetName(L"FBXVertexBuffer");

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = size;
	m_vertexBufferView.StrideInBytes = stride;
}

void FBXVertexBuffer::Copy(void* srcVertices)
{
	uint8_t* data;
	m_vertexBuffer->Map(0, nullptr, (void**)&data);
	memcpy(data, srcVertices, m_vertexBufferView.SizeInBytes);
	m_vertexBuffer->Unmap(0, nullptr);
}
