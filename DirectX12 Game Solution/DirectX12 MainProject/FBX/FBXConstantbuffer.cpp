
#include "FBXConstantbuffer.h"

#include "Base/pch.h"
#include "Base/dxtk.h"

FBXConstantBuffer::~FBXConstantBuffer()
{
	CD3DX12_RANGE readRange(0, 0);
	for (auto& cb : m_constantBuffer) {
		if (cb != nullptr) {
			cb->Unmap(0, &readRange);
			cb->Release();
		}
	}
}

void FBXConstantBuffer::Init(int size, void* srcData)
{
	m_size = size;

	m_allocSize = (size + 256) & 0xFFFFFF00;

	int bufferNo = 0;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(m_allocSize);
	for (auto& cb : m_constantBuffer) {
		DXTK->Device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&cb)
		);
		{
			CD3DX12_RANGE readRange(0, 0);
			cb->Map(0, &readRange, reinterpret_cast<void**>(&m_constBufferCPU[bufferNo]));
		}
		if (srcData != nullptr) {
			memcpy(m_constBufferCPU[bufferNo], srcData, m_size);
		}
		bufferNo++;
	}

	m_isValid = true;

}

void FBXConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle,int bufferNo)
{
	//D3Dデバイスを取得。
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = m_constantBuffer[bufferNo]->GetGPUVirtualAddress();
	desc.SizeInBytes = m_allocSize;
	DXTK->Device->CreateConstantBufferView(&desc, descriptorHandle);
}

void FBXConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
{
	auto backBufferIndex = DXTK->GetBackBufferIndex();
	RegistConstantBufferView(descriptorHandle, backBufferIndex);
}

void FBXConstantBuffer::CopyToVRAM(void* data)
{
	auto backBufferIndex = DXTK->GetBackBufferIndex();
	memcpy(m_constBufferCPU[backBufferIndex], data, m_size);
}


