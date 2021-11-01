//
// MainScene.h
//

#pragma once

#include "Scene.h"
#include "FBX/FBXMain.h"


using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;
using namespace DirectX;

class MainScene final : public Scene {
public:
	MainScene();
	virtual ~MainScene() { Terminate(); }

	MainScene(MainScene&&) = default;
	MainScene& operator= (MainScene&&) = default;

	MainScene(MainScene const&) = delete;
	MainScene& operator= (MainScene const&) = delete;

	// These are the functions you will implement.
	void Initialize() override;
	void LoadAssets() override;

	void Terminate() override;

	void OnDeviceLost() override;
	void OnRestartSound() override;

	NextScene Update(const float deltaTime) override;
	void Render() override;

private:
	struct Color
	{
		float r;
		float g;
		float b;
		float a;

		Color()
		{
			r = g = b = a = 1.0f;
		}
	};

	struct MyVertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		Color color;
		XMFLOAT2 uv;
		//float boneIndex[4] = {};//ボーン　番号
		//float boneWeight[4] = {};//ボーン　重み
	};

	struct MyMaterial
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		std::string texture;

		MyMaterial()
		{
			ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		void SetAmbient(float r, float g, float b, float factor)
		{
			ambient.x = r;
			ambient.y = g;
			ambient.z = b;
			ambient.w = factor;
		}

		void SetDiffuse(float r, float g, float b, float factor)
		{
			diffuse.x = r;
			diffuse.y = g;
			diffuse.z = b;
			diffuse.w = factor;
		}

		void SetSpecular(float r, float g, float b, float factor)
		{
			specular.x = r;
			specular.y = g;
			specular.z = b;
			specular.w = factor;
		}
	};

	struct constBuf
	{
		XMMATRIX view;
		//XMMATRIX bone[255];
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
	};

	struct MyMesh
	{
		ComPtr<ID3D12Resource>			vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;
		ComPtr<ID3D12Resource>		indexBuffer;
		D3D12_INDEX_BUFFER_VIEW		indexBufferView;
		std::vector<MyVertex>	vertices;
		std::vector<DWORD>	indeces;
		std::string			materialName;

	};

	bool CreateMesh(MyMesh& mesh_data, FbxMesh* mesh);
	bool CreateMaterial(MyMesh& mesh_data, FbxMesh* node);
	void CollectMeshNode(FbxNode* node, std::map<std::string, FbxNode*>& list);
	void CreateBone(int count,FbxMesh* pFbxMesh);
	void LoadMaterial(FbxSurfaceMaterial* material);
	void GetTexture(FbxSurfaceMaterial* material);
	SimpleMath::Matrix ConvertMatrix(const FbxMatrix& fbxmat);

	//ComPtr<ID3D12Resource>			vertexBuffer;
	//D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;

	//ComPtr<ID3D12Resource>		indexBuffer;
	//D3D12_INDEX_BUFFER_VIEW		indexBufferView;

	ComPtr<ID3DBlob>			vsBlob;
	ComPtr<ID3DBlob>			psBlob;

	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;

	unique_ptr<DescriptorHeap>  resourceDescriptors;
	ComPtr<ID3D12Resource>      texture;

	ComPtr<ID3D12Resource> constantBuffer;
	SimpleMath::Matrix worldTransform;
	DX12::CAMERA mainCamera;

	std::vector<MyVertex> pmdVertices;
	std::vector<FbxCluster*> _clusters;
	std::vector<SimpleMath::Matrix> _bones;
	std::vector<MyMesh> _meshData;

	std::map<std::string,MyMaterial>	_materials;
	std::vector<std::string>			_materialName;

	FbxAnimEvaluator* _animEvaluator;

	int _numBones = 0;


	LoadFBXFile m_FbxFile;

};