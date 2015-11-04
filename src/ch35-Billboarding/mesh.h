#ifndef MESH_H
#define MESH_H

#include <windows.h>
#include <DirectXMath.h> using namespace DirectX;

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace MeshStruct
{
    struct Vertex 
    {
    	XMFLOAT3 Position;
    	XMFLOAT3 Normal;
    	XMFLOAT2 TexCoords;
    	XMFLOAT3 Tangent;
    	XMFLOAT3 BiTangent;
    };
    
    
    struct Material
    {
    	Material()
    	{
    		ambient   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    		diffuse   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    		specular  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    		emissive  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    		// shininess = 32.0f;
    	}
    	Material (const Material &mat)
    	{
    		this->ambient   = mat.ambient  ;
    		this->diffuse   = mat.diffuse  ;
    		this->specular  = mat.specular ;
    		this->emissive  = mat.emissive ;
    		//this->shininess = mat.shininess;
    	}
    	Material &operator = (const Material &mat)
    	{
    		this->ambient   = mat.ambient  ;
    		this->diffuse   = mat.diffuse  ;
    		this->specular  = mat.specular ;
    		this->emissive  = mat.emissive ;
    		//this->shininess = mat.shininess;
    
    		return *this;
    	}
    
    	XMFLOAT4 ambient;
    	XMFLOAT4 diffuse;
    	XMFLOAT4 specular;
    	XMFLOAT4 emissive;
    	//float shininess;
    };
    
    struct Texture 
    {
    	std::string type;
    	aiString path;
    };


} //

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<MeshStruct::Vertex> VertexData;
	std::vector<DWORD> IndexData;
	std::vector<MeshStruct::Texture> textures;
	MeshStruct::Material mat;

	Mesh() {}

	Mesh(std::vector<MeshStruct::Vertex> vertices, std::vector<DWORD> indices, std::vector<MeshStruct::Texture> textures, MeshStruct::Material mat)
	{
		this->VertexData = vertices;
		this->IndexData = indices;
		this->textures = textures;
		this->mat = mat;
	}

public:
	int m_VertexCount;
	int m_IndexCount;
};

#endif