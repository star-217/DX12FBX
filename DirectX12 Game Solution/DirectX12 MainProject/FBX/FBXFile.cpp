#include "FBXMain.h"
#include "FBXFile.h"


void LoadFBXFile::InitModel(const char* name)
{
	m_manager = FbxManager::Create();

	m_ios = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(m_ios);
	m_scene = FbxScene::Create(m_manager, "");

	m_importer = FbxImporter::Create(m_manager, "");
	m_importer->Initialize(name, -1, m_manager->GetIOSettings());
	m_importer->Import(m_scene);

	FbxGeometryConverter geometryConverter(m_manager);
	geometryConverter.Triangulate(m_scene, true);

	SMesh mesh = {};

	std::map<std::string, FbxNode*> mesh_node_list;
	// メッシュNodeを探す
	CollectMeshNode(m_scene->GetRootNode(), mesh_node_list);

	int material_num = m_scene->GetSrcObjectCount<FbxSurfaceMaterial>();

	for (int i = 0; i < material_num; i++)
	{
		LoadMaterial(m_scene->GetSrcObject<FbxSurfaceMaterial>(i));
	}

	int mesh_count = 0;;
	for (auto data : mesh_node_list)
	{
		// mesh作成
		CreateMesh(mesh,data.second->GetMesh());
	}


	m_importer->Destroy();
	m_scene->Destroy();
	m_manager->Destroy();


	m_manager->Destroy();
}

bool LoadFBXFile::CreateMesh(SMesh& data,FbxMesh* mesh)
{
	// 頂点バッファの取得
	FbxVector4* vertices = mesh->GetControlPoints();
	// インデックスバッファの取得
	int* indices = mesh->GetPolygonVertices();
	// 頂点座標の数の取得
	int polygon_vertex_count = mesh->GetPolygonVertexCount();

	// GetPolygonVertexCount => 頂点数
	for (int i = 0; i < polygon_vertex_count; i++)
	{
		SVertex vertex;
		// インデックスバッファから頂点番号を取得
		int index = indices[i];

		// 頂点座標リストから座標を.
		vertex.position.x = (float)-vertices[index][0];
		vertex.position.y = (float)vertices[index][1];
		vertex.position.z = (float)vertices[index][2];

		// 追加
		data.vertices.push_back(vertex);
	}

	FbxArray<FbxVector4> normals;
	// 法線リストの取得
	mesh->GetPolygonVertexNormals(normals);

	// 法線設定
	for (int i = 0; i < normals.Size(); i++)
	{
		data.vertices[i].normal.x = (float)-normals[i][0];
		data.vertices[i].normal.y = (float)normals[i][1];
		data.vertices[i].normal.z = (float)normals[i][2];
	}

	// ポリゴン数の取得
	int polygon_count = mesh->GetPolygonCount();

	// ポリゴンの数だけ連番として保存する
	for (int i = 0; i < polygon_count; i++)
	{
		data.indeces.push_back(i * 3 + 2);
		data.indeces.push_back(i * 3 + 1);
		data.indeces.push_back(i * 3);
	}

	FbxStringList uvset_names;
	mesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;

	mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++)
	{
		data.vertices[i].uv.x = (float)uv_buffer[i][0];
		data.vertices[i].uv.y = (float)uv_buffer[i][1];
	}

	auto a = data.vertices[uv_buffer.Size() - 1];

	return true;
}

void LoadFBXFile::CollectMeshNode(FbxNode* node, std::map<std::string, FbxNode*>& list)
{
	for (int i = 0; i < node->GetNodeAttributeCount(); i++)
	{
		FbxNodeAttribute* attribute = node->GetNodeAttributeByIndex(i);

		// Attributeがメッシュなら追加
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

void LoadFBXFile::LoadMaterial(FbxSurfaceMaterial* material)
{
	SMaterial entry_material;

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


	GetTexture(material);

}

void LoadFBXFile::GetTexture(FbxSurfaceMaterial* material)
{
	auto prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

	int textureNum = prop.GetSrcObjectCount<FbxFileTexture>();
	if (textureNum > 0)
	{
		texture = prop.GetSrcObject<FbxFileTexture>(0);
	}
	else
	{
		int layer_num = prop.GetSrcObjectCount<FbxLayeredTexture>();

		if (layer_num > 0)
			texture = prop.GetSrcObject<FbxFileTexture>(0);

	}

	std::string file_path = texture->GetRelativeFileName();
	CharacterConversion(file_path);
}

void LoadFBXFile::CharacterConversion(std::string& path)
{
	char* name;
	size_t size = 0;

	FbxUTF8ToAnsi(path.c_str(), name, &size);
}
