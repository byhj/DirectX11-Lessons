#ifndef MODEL_H
#define MODEL_H

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

#include "Mesh.h"

int TextureFromFile(const char* path, std::string directory);

class Model
{
public:
	Model()
	{
		m_VertexCount = 0;
		m_IndexCount  = 0;
	}

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<MeshStruct::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	int GetVertexCount()
	{
		return m_VertexCount;
	}

	int  GetIndexCount()
	{
		return m_IndexCount;
	}
	std::vector<XMFLOAT3> GetPos()
	{
		return m_vPos;
	}

	std::vector<MeshStruct::Vertex> GetVertexData()
	{
		return m_VertexData;
	}

	std::vector<DWORD>  GetIndexData()
	{
		return m_IndexData;
	}
	void setColor(aiColor3D &c, XMFLOAT4 &m);
	void setBlend(float blend, MeshStruct::Material &mat);

private:

	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<MeshStruct::Texture> textures_loaded;
	std::vector<MeshStruct::Vertex> m_VertexData;
	std::vector<DWORD>  m_IndexData;

	std::vector<XMFLOAT3> m_vPos;
	int m_VertexCount = 0;
	int m_IndexCount = 0;
};

#endif