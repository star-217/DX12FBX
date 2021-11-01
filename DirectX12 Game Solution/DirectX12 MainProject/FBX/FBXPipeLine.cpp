#include "FBXMain.h"
#include "FBXPipeLine.h"

void PipelineState::Init(D3D12_GRAPHICS_PIPELINE_STATE_DESC desc)
{
	auto re = DXTK->Device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(re)) {
		MessageBoxA(nullptr, "�p�C�v���C���X�e�[�g�̍쐬�Ɏ��s���܂����B\n", "�G���[", MB_OK);
		std::abort();
	}


}


void PipelineState::Init(D3D12_COMPUTE_PIPELINE_STATE_DESC desc)
{
	auto re = DXTK->Device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(re)) {
		MessageBoxA(nullptr, "�p�C�v���C���X�e�[�g�̍쐬�Ɏ��s���܂����B\n", "�G���[", MB_OK);
		std::abort();
	}
}
