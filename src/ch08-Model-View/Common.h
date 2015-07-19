#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <xnamath.h>

namespace byhj
{

struct Vertex	
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float cr, float cg, float cb, float ca)
		: pos(x,y,z), color(cr, cg, cb, ca){}

	XMFLOAT3 pos;
	XMFLOAT4 color;
};

struct MatrixBuffer
{
	XMFLOAT4X4  Model;
	XMFLOAT4X4  View;
	XMFLOAT4X4  Proj;
};


}
#endif