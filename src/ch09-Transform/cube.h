#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <d3d11.h>
#include <xnamath.h>

#include "Common.h"
#include "d3d/d3dShader.h"

namespace byhj
{

class Cube
{

public:
	Cube() 
	{
		m_VertexCount = 0;
		m_IndexCount  = 0;
		m_pVertexBuffer = nullptr;
		m_pIndexBuffer  = nullptr;
	}
	~Cube() {}

public:
	void Init(ID3D11Device *pD3D11Device, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const byhj::MatrixBuffer &matrix);
	void Shutdown();

private:

	void init_buffer(ID3D11Device *pD3D11Device);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);

	int m_VertexCount;
	int m_IndexCount;
    byhj::MatrixBuffer m_cbMatrix;
	Shader TestShader;
	ID3D11Buffer  *m_pVertexBuffer;
	ID3D11Buffer  *m_pIndexBuffer;
	ID3D11Buffer  *m_pMVPBuffer;
};


}


#endif