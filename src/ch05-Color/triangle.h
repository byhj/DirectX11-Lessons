#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <d3d11.h>
#include <xnamath.h>

#include "d3d/d3dShader.h"

namespace byhj
{

class Triangle
{

public:
	Triangle()  {}
	~Triangle() {}

public:
	void Init(ID3D11Device *pD3D11Device, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext);
	void Shutdown();

private:
	struct Vertex
	{
      XMFLOAT3 Position;
	  XMFLOAT4 Color;
	};

	void init_buffer(ID3D11Device *pD3D11Device);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);


	Shader TestShader;
	int m_VertexCount = 0;
	int m_IndexCount  = 0;
	ID3D11Buffer  *m_pVertexBuffer = nullptr;
};


}


#endif