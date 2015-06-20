#ifndef D3DMODEL_H
#define D3DMODEL_H

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "d3dMesh.h"

int TextureFromFile(const char* path, std::string directory);

class D3DModel
{
public:
	D3DModel(){}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX MVP)
	{
		ModelShader.use(pD3D11DeviceContext);
		for (int i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Render(pD3D11DeviceContext, MVP);
	}
	
	//Loading the model form file if support this format
	void loadModel(std::string path);
	void initModel(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
	
   // Processes a node in a recursive fashion. Processes each individual mesh located at 
	//the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);

	//Processes a mesh and return the data information
	D3DMesh processMesh(aiMesh* mesh, const aiScene* scene);
	
	//Load the texture for material
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	void D3DModel::init_shader(ID3D11Device *pD3D11Device, HWND hWnd);

private:

	//One model may include many meshes
	std::vector<D3DMesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;	

	ID3D11Device *pD3D11Device;
	ID3D11DeviceContext *pD3D11DeviceContext; 
	HWND hWnd;
	Shader ModelShader;
};

void D3DModel::initModel(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	this->pD3D11Device = pD3D11Device;
	this->pD3D11DeviceContext = pD3D11DeviceContext;
	this->hWnd = hWnd;
	init_shader(pD3D11Device, hWnd);
}

void D3DModel::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[3];

	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "NORMAL";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	pInputLayoutDesc[2].SemanticName         = "TEXCOORD";
	pInputLayoutDesc[2].SemanticIndex        = 0;
	pInputLayoutDesc[2].Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc[2].InputSlot            = 0;
	pInputLayoutDesc[2].AlignedByteOffset    = 24;
	pInputLayoutDesc[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[2].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	ModelShader.init(pD3D11Device, hWnd);
	ModelShader.attachVS(L"model.vsh", pInputLayoutDesc, numElements);
	ModelShader.attachPS(L"model.psh");
	ModelShader.end();
}

void D3DModel::loadModel(std::string path)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->processNode(scene->mRootNode, scene);
}

std::vector<Texture>  D3DModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
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
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

D3DMesh D3DModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//Mesh Data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned long> indices;
	std::vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		XMFLOAT3 temp; // We declare a placeholder std::vector since assimp uses its own std::vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder XMFloat3 first.

		// Positions
		temp.x = mesh->mVertices[i].x;
		temp.y = mesh->mVertices[i].y;
		temp.z = mesh->mVertices[i].z;
		vertex.Position = temp;

		// Normals
		temp.x = mesh->mNormals[i].x;
		temp.y = mesh->mNormals[i].y;
		temp.z = mesh->mNormals[i].z;
		vertex.Normal = temp;

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
	}

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices std::vector
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Return a mesh object created from the extracted mesh data
	return D3DMesh(vertices, indices, textures, pD3D11Device, pD3D11DeviceContext, hWnd);
}

void D3DModel::processNode(aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}

}

int TextureFromFile(const char* path, std::string directory)
{
	return 0;
}
#endif