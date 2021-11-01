#pragma once

#include <fbxsdk.h>

using namespace DirectX::SimpleMath;

class LoadFBXFile{
public:
	struct SMaterial
	{
		Vector4 diffuse;
		Vector4 ambient;

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
	};

	struct SVertex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 uv;
	};

	struct SMesh
	{
		std::vector< SVertex >	vertices;
		std::vector< int >		indeces;
	};

	void InitModel(const char* name);

private:
	bool CreateMesh(SMesh& data,FbxMesh* mesh);
	void CollectMeshNode(FbxNode* node, std::map<std::string, FbxNode*>& list);
	void LoadMaterial(FbxSurfaceMaterial* material);
	void GetTexture(FbxSurfaceMaterial* material);
	void CharacterConversion(std::string& path);

private:
	FbxManager* m_manager;
	FbxIOSettings* m_ios;
	FbxScene* m_scene;
	FbxImporter* m_importer;
	FbxFileTexture* texture;
};