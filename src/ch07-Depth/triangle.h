#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <d3d11.h>
#include <DirectXMath.h> 
using namespace DirectX;

#include "d3d/Shader.h"

namespace byhj
{

class Triangle
{

public:
	Triangle() 	 = default;
	~Triangle()  = default;

public:
	void Init(ID3D11Device *pD3D11Device, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext);
	void Shutdown();

private:
	struct Vertex	
	{
		Vertex(){}
		Vertex(float x, float y, float z,
			float cr, float cg, float cb, float ca)
			: pos(x,y,z), color(cr, cg, cb, ca){}

		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	void init_buffer(ID3D11Device *pD3D11Device);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);

	int m_VertexCount = 0;
	int m_IndexCount  = 0;

	d3d::Shader TestShader;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
};


}


#endif