#pragma once

#include "Base/pch.h"
#include "Base/dxtk.h"

class FBXVertexBuffer
{
public:
	~FBXVertexBuffer();

	void Init(int size,int stride);

	void Copy(void* srcVertices);

private:
	ID3D12Resource* m_vertexBuffer = nullptr;		//頂点バッファ。
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;	//頂点バッファビュー。
};