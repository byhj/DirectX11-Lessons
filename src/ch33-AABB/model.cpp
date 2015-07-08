#include "model.h"

void Model::loadModel(std::string path)
{

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->processNode(scene->mRootNode, scene);

}

std::vector<MeshStruct::Texture>  Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<MeshStruct::Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}

		if (!skip)
		{   // If texture hasn't been loaded already, load it
			MeshStruct::Texture texture;
			//texture.pTexture = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Model::setColor(aiColor3D &c, XMFLOAT4 &m)
{
	m.x = c[0];
	m.y = c[1];
	m.z = c[2];
}

void Model::setBlend(float blend, MeshStruct::Material &mat)
{
	mat.ambient.w = blend;
	mat.specular.w = blend;
	mat.diffuse.w = blend;
	mat.emissive.w = blend;
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//Mesh Data to fill
	std::vector<MeshStruct::Vertex> vertices;
	std::vector<DWORD> indices;
	std::vector<MeshStruct::Texture> textures;
	std::vector<MeshStruct::Material> MaterialData;

	XMVECTOR v[3];
	XMVECTOR uv[3];

	// Walk through each of the mesh's vertices
	for (int i = 0, j = 0; i < mesh->mNumVertices; i++)
	{
		MeshStruct::Vertex vertex;
		XMFLOAT3 pos; // We declare a placeholder std::vector since assimp uses its own std::vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder XMFloat3 first.
		XMFLOAT3 normal;

		// Positions
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		vertex.Position = pos;
		m_vPos.push_back(pos);

		// Normals
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vertex.Normal = normal;

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			XMFLOAT2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = XMFLOAT2(0.0f, 0.0f);

		//Process one Vertex
		vertices.push_back(vertex);

		v[j]  = XMLoadFloat3(&vertex.Position);
		uv[j] = XMLoadFloat2(&vertex.TexCoords );
		if ( (j+1) % 3 == 0)
		{
			// Edges of the triangle : postion delta
			XMVECTOR deltaPos1 = v[1]  - v[0];
			XMVECTOR deltaPos2 = v[2]  - v[0];
			XMVECTOR deltaUV1  = uv[1] - uv[0];
			XMVECTOR deltaUV2  = uv[2] - uv[0];

		    float uv1x = XMVectorGetX(deltaUV1);
			float uv1y = XMVectorGetY(deltaUV1);
			float uv2x = XMVectorGetX(deltaUV2);
			float uv2y = XMVectorGetY(deltaUV2);

			float r = 1.0f / (uv1x * uv2y - uv1y * uv2x);
			XMVECTOR tangent = (deltaPos1 * uv2y   - deltaPos2 * uv1y) * r;
			XMVECTOR bitangent = (deltaPos2 * uv1x   - deltaPos1 * uv2x) * r;

			XMStoreFloat3(&vertices[i].Tangent, tangent);
			XMStoreFloat3(&vertices[i-1].Tangent, tangent);
			XMStoreFloat3(&vertices[i-2].Tangent, tangent);

			XMStoreFloat3(&  vertices[i].BiTangent, bitangent);
			XMStoreFloat3(&vertices[i-1].BiTangent, bitangent);
			XMStoreFloat3(&vertices[i-2].BiTangent, bitangent);
			j = 0;
		}
		else
			++j;
	}

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (int j = 0; j < static_cast<int>(face.mNumIndices); j++)
			indices.push_back(face.mIndices[j]);
	}
	MeshStruct::Material mat;

	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		int tt = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		aiColor3D ambient;
		if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) )
			setColor(ambient, mat.ambient);
		//std::cout << mat.ambient.w << std::endl;
		aiColor3D diffuse;
		if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) )
			setColor(diffuse, mat.diffuse);

		aiColor3D  speucular;
		if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, speucular) )
			setColor(speucular, mat.specular);

		aiColor3D  emissive;
		if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) )
			setColor(emissive, mat.emissive);

		//float shininess = 0.0;
		//if(AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
		//	mat.shininess = shininess;

		float blend;
		material->Get(AI_MATKEY_OPACITY , blend);
		if (blend < 1.0f)
			setBlend(blend, mat);
		//std::cout << mat.ambient.w << std::endl;

		// 1. Diffuse maps
		std::vector<MeshStruct::Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 2. Specular maps
		std::vector<MeshStruct::Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3.normal maps
		std::vector<MeshStruct::Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	m_VertexCount += vertices.size();
	m_IndexCount  += indices.size();
	m_VertexData.insert(m_VertexData.end(), vertices.begin(), vertices.end());
	m_IndexData.insert(m_IndexData.end(), indices.begin(), indices.end());

	return Mesh(vertices, indices, textures, mat);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (int i = 0; i < node->mNumChildren; i++)
		this->processNode(node->mChildren[i], scene);

}
