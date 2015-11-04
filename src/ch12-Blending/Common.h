#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <DirectXMath.h> using namespace DirectX;

namespace byhj
{

struct Vertex	
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x,y,z), texCoord(u, v){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

struct MatrixBuffer
{
	XMFLOAT4X4  Model;
	XMFLOAT4X4  View;
	XMFLOAT4X4  Proj;
};


}
#endif