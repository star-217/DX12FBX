//
// MainScene.cpp
//

#include "Base/pch.h"
#include "Base/dxtk.h"
#include "SceneFactory.h"

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

// Initialize member variables.
MainScene::MainScene()
{

}

// Initialize a variable and audio resources.
void MainScene::Initialize()
{
	auto scale = SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	auto rote = SimpleMath::Matrix::CreateFromYawPitchRoll(
		XMConvertToRadians(180),
		XMConvertToRadians(0),
		XMConvertToRadians(0)
	);
	auto pos = SimpleMath::Matrix::CreateTranslation(0, -100, 0);
	worldTransform = scale * rote * pos;

	mainCamera.SetView(
		SimpleMath::Vector3(0, -5, -200),
		SimpleMath::Vector3(0, 0, 0)
	);
	mainCamera.SetPerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		16.0f / 9.0f,
		1.0f, 10000.0f
	);
}

// Allocate all memory the Direct3D and Direct2D resources.
void MainScene::LoadAssets()
{

	auto file_name = "UnityCHanKAGURA.fbx";

	// FbxManager�쐬
	FbxManager* fbx_manager = fbxsdk::FbxManager::Create();
	if (fbx_manager == nullptr)
	{
		return;
	}

	// FbxImporter�쐬
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	if (fbx_importer == nullptr)
	{
		fbx_manager->Destroy();
		return;
	}

	// FbxScene�쐬
	FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "");
	if (fbx_scene == nullptr)
	{
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		return;
	}

	// File��������
	fbx_importer->Initialize(file_name);
	// scene�ɃC���|�[�g
	fbx_importer->Import(fbx_scene);

	FbxGeometryConverter converter(fbx_manager);
	// �SMesh����
	//converter.SplitMeshesPerMaterial(fbx_scene, true);

	// �|���S�����O�p�`�ɂ���
	converter.Triangulate(fbx_scene, true);

	std::map<std::string, FbxNode*> mesh_node_list;
	// ���b�V��Node��T��
	CollectMeshNode(fbx_scene->GetRootNode(), mesh_node_list);

	int material_num = fbx_scene->GetSrcObjectCount<FbxSurfaceMaterial>();

	for (int i = 0; i < material_num; i++)
	{
		LoadMaterial(fbx_scene->GetSrcObject<FbxSurfaceMaterial>(i));
	}

	int mesh_count = 0;;
	for (auto data : mesh_node_list)
	{
		MyMesh mesh_data;
		// mesh�쐬
		CreateMesh(mesh_data, data.second->GetMesh());
		CreateMaterial(mesh_data, data.second->GetMesh());
		//CreateBone(mesh_count, data.second->GetMesh());
		_meshData.push_back(mesh_data);
		mesh_count++;
	}

	_animEvaluator = fbx_scene->GetAnimationEvaluator();

	fbx_importer->Destroy();
	fbx_scene->Destroy();
	fbx_manager->Destroy();

	HRESULT result;
	for (int i = 0; i < _meshData.size(); ++i) {

		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resdesc.Width = sizeof(MyVertex) * _meshData[i].vertices.size();
		resdesc.Height = 1;
		resdesc.DepthOrArraySize = 1;
		resdesc.MipLevels = 1;
		resdesc.Format = DXGI_FORMAT_UNKNOWN;
		resdesc.SampleDesc.Count = 1;
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		result = DXTK->Device->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(_meshData[i].vertexBuffer.ReleaseAndGetAddressOf()));
		DX::ThrowIfFailed(result);

		void* map_addr = nullptr;
		_meshData[i].vertexBuffer->Map(0, nullptr, &map_addr);
		CopyMemory(map_addr, _meshData[i].vertices.data(), sizeof(MyVertex) * _meshData[i].vertices.size());
		_meshData[i].vertexBuffer->Unmap(0, nullptr);

		_meshData[i].vertexBufferView.BufferLocation = _meshData[i].vertexBuffer->GetGPUVirtualAddress();
		_meshData[i].vertexBufferView.SizeInBytes = sizeof(MyVertex) * _meshData[i].vertices.size();
		_meshData[i].vertexBufferView.StrideInBytes = sizeof(MyVertex);
	}

	for (int i = 0; i < _meshData.size(); ++i)
	{
		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resdesc.Width = sizeof(_meshData[i].indeces.at(0)) * _meshData[i].indeces.size();
		resdesc.Height = 1;
		resdesc.DepthOrArraySize = 1;
		resdesc.MipLevels = 1;
		resdesc.Format = DXGI_FORMAT_UNKNOWN;
		resdesc.SampleDesc.Count = 1;
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		//�C���f�b�N�X�o�b�t�@�[
		result = DXTK->Device->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(_meshData[i].indexBuffer.ReleaseAndGetAddressOf()));
		DX::ThrowIfFailed(result);

		void* map_addr = nullptr;
		_meshData[i].indexBuffer->Map(0, nullptr, &map_addr);
		CopyMemory(map_addr, _meshData[i].indeces.data(), sizeof(_meshData[i].indeces.at(0)) * _meshData[i].indeces.size());
		_meshData[i].indexBuffer->Unmap(0, nullptr);

		_meshData[i].indexBufferView.BufferLocation = _meshData[i].indexBuffer->GetGPUVirtualAddress();
		_meshData[i].indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		_meshData[i].indexBufferView.SizeInBytes = sizeof(_meshData[i].indeces.at(0)) * _meshData[i].indeces.size();
	}

	static int frame = 0;
	frame += 1;
	if (frame > 360)//���̃T���v��FBX���쐬���鎞��360�t���[���ō쐬��������
	{
		frame = 0;
	}

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(XMMATRIX) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	resourceDescriptors = make_unique<DescriptorHeap>(DXTK->Device, 1);
	//�R���X�^���g�o�b�t�@�[
	result = DXTK->Device->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constantBuffer.ReleaseAndGetAddressOf())
	);
	DX::ThrowIfFailed(result);

	// �R���X�^���g�o�b�t�@�r���[
	auto desc_addr = resourceDescriptors->GetFirstCpuHandle();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
	cbv_desc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
	cbv_desc.SizeInBytes = (UINT)constantBuffer->GetDesc().Width;
	DXTK->Device->CreateConstantBufferView(&cbv_desc, desc_addr);

	// �R���X�^���g�o�b�t�@�ւ̏�������
	XMMATRIX * map_buffer = nullptr;
	constantBuffer->Map(0, nullptr, (void**)&map_buffer);
	*map_buffer = worldTransform * mainCamera->GetViewProjectionMatrix();
	constantBuffer->Unmap(0, nullptr);

	ComPtr<ID3DBlob> error_blob;

	result = D3DCompileFromFile(
		L"Shaders/VS.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		vsBlob.ReleaseAndGetAddressOf(),
		error_blob.ReleaseAndGetAddressOf()
	);
	DX::ThrowIfFailed(result);

	result = D3DCompileFromFile(
		L"Shaders/PS.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		psBlob.ReleaseAndGetAddressOf(),
		error_blob.ReleaseAndGetAddressOf()
	);
	DX::ThrowIfFailed(result);

	// ���[�g�V�O�l�`��
	D3D12_DESCRIPTOR_RANGE descRange = {};
	descRange.NumDescriptors = 1;
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descRange.BaseShaderRegister = 0;
	descRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.DescriptorTable.pDescriptorRanges = &descRange;
	rootparam.DescriptorTable.NumDescriptorRanges = 1;
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootparam;
	rootSignatureDesc.NumParameters = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	result = DirectX::CreateRootSignature(
		DXTK->Device,
		&rootSignatureDesc,
		rootSignature.ReleaseAndGetAddressOf()
	);
	DX::ThrowIfFailed(result);

	// �p�C�v���C���X�e�[�g
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = nullptr;
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	renderTargetBlendDesc.LogicOpEnable = false;
	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	gpipeline.RasterizerState.MultisampleEnable = false;
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpipeline.RasterizerState.DepthClipEnable = true;
	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DepthStencilState.StencilEnable = false;

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",
			0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL",
			0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"COLOR",
			0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	"TEXTURE",
			0, DXGI_FORMAT_R32G32_FLOAT,
			0,	D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		/*{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },*/

	};
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;

	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;

	gpipeline.pRootSignature = rootSignature.Get();
	result = DXTK->Device->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf())
	);
	DX::ThrowIfFailed(result);
}

// Releasing resources required for termination.
void MainScene::Terminate()
{
	DXTK->ResetAudioEngine();
	DXTK->WaitForGpu();

	// TODO: Add your Termination logic here.

}

// Direct3D resource cleanup.
void MainScene::OnDeviceLost()
{

}

// Restart any looped sounds here
void MainScene::OnRestartSound()
{

}

// Updates the scene.
NextScene MainScene::Update(const float deltaTime)
{
	// If you use 'deltaTime', remove it.
	UNREFERENCED_PARAMETER(deltaTime);

	// TODO: Add your game logic here.



	return NextScene::Continue;
}

// Draws the scene.
void MainScene::Render()
{
	// TODO: Add your rendering code here.
	DXTK->ResetCommand();
	DXTK->ClearRenderTarget(Colors::Black);

	DXTK->CommandList->SetGraphicsRootSignature(rootSignature.Get());
	DXTK->CommandList->SetPipelineState(pipelineState.Get());
	DXTK->CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto heapes = resourceDescriptors->Heap();
	DXTK->CommandList->SetDescriptorHeaps(1, &heapes);
	DXTK->CommandList->SetGraphicsRootDescriptorTable(0, heapes->GetGPUDescriptorHandleForHeapStart());

	// �R���X�^���g�o�b�t�@�ւ̏�������
	constBuf* map_buffer = nullptr;
	constantBuffer->Map(0, nullptr, (void**)&map_buffer);
	auto i = 0;

	for (auto data : _meshData) {
		DXTK->CommandList->IASetVertexBuffers(0, 1, &data.vertexBufferView);
		DXTK->CommandList->IASetIndexBuffer(&data.indexBufferView);
		DXTK->CommandList->DrawIndexedInstanced(data.indeces.size(), 1, 0, 0, 0);
		map_buffer->view = worldTransform * mainCamera->GetViewProjectionMatrix();
		map_buffer->ambient = _materials[data.materialName].ambient;
		map_buffer->diffuse = _materials[data.materialName].diffuse;
		i++;
	}
	constantBuffer->Unmap(0, nullptr);

	DXTK->ExecuteCommandList();
}

bool MainScene::CreateMesh(MyMesh& mesh_data, FbxMesh* mesh)
{
	// ���_�o�b�t�@�̎擾
	FbxVector4* vertices = mesh->GetControlPoints();
	// �C���f�b�N�X�o�b�t�@�̎擾
	int* indices = mesh->GetPolygonVertices();
	// ���_���W�̐��̎擾
	int polygon_vertex_count = mesh->GetPolygonVertexCount();

	// GetPolygonVertexCount => ���_��
	for (int i = 0; i < polygon_vertex_count; i++)
	{
		MyVertex vertex;
		// �C���f�b�N�X�o�b�t�@���璸�_�ԍ����擾
		int index = indices[i];

		// ���_���W���X�g������W��.
		vertex.pos.x = (float)-vertices[index][0];
		vertex.pos.y = (float)vertices[index][1];
		vertex.pos.z = (float)vertices[index][2];

		// �ǉ�
		mesh_data.vertices.push_back(vertex);
	}

	FbxArray<FbxVector4> normals;
	// �@�����X�g�̎擾
	mesh->GetPolygonVertexNormals(normals);

	// �@���ݒ�
	for (int i = 0; i < normals.Size(); i++)
	{
		mesh_data.vertices[i].normal.x = (float)-normals[i][0];
		mesh_data.vertices[i].normal.y = (float)normals[i][1];
		mesh_data.vertices[i].normal.z = (float)normals[i][2];
	}

	// �|���S�����̎擾
	int polygon_count = mesh->GetPolygonCount();

	// �|���S���̐������A�ԂƂ��ĕۑ�����
	for (int i = 0; i < polygon_count; i++)
	{
		mesh_data.indeces.push_back(i * 3 + 2);
		mesh_data.indeces.push_back(i * 3 + 1);
		mesh_data.indeces.push_back(i * 3);
	}

	//���_�J���[
	int color_count = mesh->GetElementVertexColorCount();
	if (color_count == 0)
		return false;

	FbxGeometryElementVertexColor* vertex_colors = mesh->GetElementVertexColor(0);

	FbxLayerElement::EMappingMode mapping_mode = vertex_colors->GetMappingMode();
	FbxLayerElement::EReferenceMode reference_mode = vertex_colors->GetReferenceMode();

	if (mapping_mode == FbxLayerElement::eByPolygonVertex)
	{
		if (reference_mode == FbxLayerElement::eIndexToDirect)
		{
			// ���_�J���[�o�b�t�@�擾
			FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
			// ���_�J���[�C���f�b�N�X�o�b�t�@�擾
			FbxLayerElementArrayTemplate<int>& indeces = vertex_colors->GetIndexArray();
			for (int i = 0; i < indeces.GetCount(); i++)
			{
				int id = indeces.GetAt(i);
				FbxColor color = colors.GetAt(id);
				mesh_data.vertices[i].color.a = (float)color.mAlpha;
				mesh_data.vertices[i].color.r = (float)color.mRed;
				mesh_data.vertices[i].color.g= (float)color.mGreen;
				mesh_data.vertices[i].color.b = (float)color.mBlue;
			}
		}
	}

	return true;
}

bool MainScene::CreateMaterial(MyMesh& mesh_data, FbxMesh* mesh)
{
	// �}�e���A����������ΏI���
	if (mesh->GetElementMaterialCount() == 0)
	{
		mesh_data.materialName = "";
		return false;
	}

	// Mesh���̃}�e���A�������擾
	FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
	int index = material->GetIndexArray().GetAt(0);
	FbxSurfaceMaterial* surface_material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);

	if (surface_material != nullptr)
	{
		mesh_data.materialName = surface_material->GetName();
	}
	else
	{
		mesh_data.materialName = "";
	}

	return true;
}

void MainScene::CollectMeshNode(FbxNode* node, std::map<std::string, FbxNode*>& list)
{
	for (int i = 0; i < node->GetNodeAttributeCount(); i++)
	{
		FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex(i);

		// Attribute�����b�V���Ȃ�ǉ�
		if (attribute->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
		{
			list[node->GetName()] = node;
			break;
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		CollectMeshNode(node->GetChild(i), list);
	}
}

void MainScene::CreateBone(int count,FbxMesh* pFbxMesh)
{
	int i, k;
	int numVertex = pFbxMesh->GetControlPointsCount();//���_��
	int* piReadCount = new int[numVertex];//�C���f�b�N�X�Əd�݂͍ő�S�B���܂œǂ񂾂��̃J�E���^�[
	ZeroMemory(piReadCount, sizeof(int) * numVertex);

	//�f�t�H�[�}�[�𓾂�
	FbxDeformer* pDeformer = pFbxMesh->GetDeformer(0);
	FbxSkin* pSkinInfo = static_cast<FbxSkin*>(pDeformer);

	//�{�[���𓾂�
	_numBones = pSkinInfo->GetClusterCount();
	_clusters.resize(_numBones);
	for (i = 0; i < _numBones; i++)
	{
		_clusters[i] = pSkinInfo->GetCluster(i);
	}

	for (i = 0; i < _numBones; i++)
	{
		int iNumIndex = _clusters[i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
		int* piIndex = _clusters[i]->GetControlPointIndices();
		double* pdWeight = _clusters[i]->GetControlPointWeights();

		//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
		//for (k = 0; k < iNumIndex; k++)
		//{
		//	for (int m = 0; m < 4; m++)//FBX��CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ��B5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi��
		//	{
		//		if (pdWeight[k] > _meshData[count].vertices[piIndex[k]].boneWeight[m])
		//		{
		//			_meshData[count].vertices[piIndex[k]].boneIndex[m] = i;
		//			_meshData[count].vertices[piIndex[k]].boneWeight[m] = pdWeight[k];
		//			break;
		//		}
		//	}
		//	if (piReadCount[piIndex[k]] < 3)
		//	{
		//		piReadCount[piIndex[k]]++;
		//	}
		//}
	}

	_bones.resize(_numBones);
	for (i = 0; i < _numBones; i++)
	{
		FbxAMatrix mat;
		_clusters[i]->GetTransformLinkMatrix(mat);

		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				_bones[i].m[y][x] = mat.Get(y, x);
			}
		}
	}
}

void MainScene::LoadMaterial(FbxSurfaceMaterial* material)
{
	MyMaterial entry_material;

	enum class MaterialOrder
	{
		Ambient,
		Diffuse,
		Specular,
		MaxOrder,
	};

	FbxDouble3 colors[(int)MaterialOrder::MaxOrder];
	FbxDouble factors[(int)MaterialOrder::MaxOrder];
	FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sAmbient);

	if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		const char* element_check_list[] =
		{
			FbxSurfaceMaterial::sAmbient,
			FbxSurfaceMaterial::sDiffuse,
		};

		const char* factor_check_list[] =
		{
			FbxSurfaceMaterial::sAmbientFactor,
			FbxSurfaceMaterial::sDiffuseFactor,
		};

		for (int i = 0; i < 2; i++)
		{
			prop = material->FindProperty(element_check_list[i]);
			if (prop.IsValid())
			{
				colors[i] = prop.Get<FbxDouble3>();

			}
			else
			{
				colors[i] = FbxDouble3(1.0, 1.0, 1.0);
			}

			prop = material->FindProperty(factor_check_list[i]);
			if (prop.IsValid())
			{
				factors[i] = prop.Get<FbxDouble>();
			}
			else
			{
				factors[i] = 1.0;
			}
		}
	}

	FbxDouble3 color = colors[(int)MaterialOrder::Ambient];
	FbxDouble factor = factors[(int)MaterialOrder::Ambient];
	entry_material.SetAmbient((float)color[0], (float)color[1], (float)color[2], (float)factor);

	color = colors[(int)MaterialOrder::Diffuse];
	factor = factors[(int)MaterialOrder::Diffuse];
	entry_material.SetDiffuse((float)color[0], (float)color[1], (float)color[2], (float)factor);

	_materials[material->GetName()] = entry_material;
	_materialName.push_back(material->GetName());

	//auto prop2 = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	//FbxFileTexture* textureData = nullptr;

	//int textureNum = prop.GetSrcObjectCount<FbxFileTexture>();
	//if (textureNum > 0)
	//{
	//	// prop����FbxFileTexture���擾
	//	textureData = prop2.GetSrcObject<FbxFileTexture>(0);
	//}
	//else
	//{
	//	// ���s������}���`�e�N�X�`���̉\�����l����FbxLayeredTexture���w�肷��
	//// FbxLayeredTexture����FbxFileTexture���擾
	//	int layerNum = prop2.GetSrcObjectCount<FbxLayeredTexture>();
	//	if (layerNum > 0)
	//	{
	//		textureData = prop2.GetSrcObject<FbxFileTexture>(0);
	//	}
	//}

	//auto filePath = textureData->GetRelativeFileName();
	//int AS = 0;


}

void MainScene::GetTexture(FbxSurfaceMaterial* material)
{

}

SimpleMath::Matrix MainScene::ConvertMatrix(const FbxMatrix& fbxmat)
{
	SimpleMath::Matrix mat;

	mat._11 = float(fbxmat[0][0]); mat._12 = float(fbxmat[1][0]); mat._13 = float(fbxmat[2][0]); mat._14 = float(fbxmat[3][0]);
	mat._21 = float(fbxmat[0][1]); mat._22 = float(fbxmat[1][1]); mat._23 = float(fbxmat[2][1]); mat._24 = float(fbxmat[3][1]);
	mat._31 = float(fbxmat[0][2]); mat._32 = float(fbxmat[1][2]); mat._33 = float(fbxmat[2][2]); mat._34 = float(fbxmat[3][2]);
	mat._41 = float(fbxmat[0][3]); mat._42 = float(fbxmat[1][3]); mat._43 = float(fbxmat[2][3]); mat._44 = float(fbxmat[3][3]);

	mat._11 = float(fbxmat[0][0]); mat._12 = float(fbxmat[0][1]); mat._13 = float(fbxmat[0][2]); mat._14 = float(fbxmat[0][3]);
	mat._21 = float(fbxmat[1][0]); mat._22 = float(fbxmat[1][1]); mat._23 = float(fbxmat[1][2]); mat._24 = float(fbxmat[1][3]);
	mat._31 = float(fbxmat[2][0]); mat._32 = float(fbxmat[2][1]); mat._33 = float(fbxmat[2][2]); mat._34 = float(fbxmat[2][3]);
	mat._41 = float(fbxmat[3][0]); mat._42 = float(fbxmat[3][1]); mat._43 = float(fbxmat[3][2]); mat._44 = float(fbxmat[3][3]);

	return mat;
}