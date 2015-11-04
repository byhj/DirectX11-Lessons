#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <d3d11.h>
#include <DirectXMath.h> using namespace DirectX;

#include "Common.h"
#include "d3d/Shader.h"

namespace byhj
{

class Cube
{

public:
	Cube() 	 = default;
	~Cube()  = default;

public:
	void Init(ID3D11Device *pD3D11Device, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const byhj::MatrixBuffer &matrix);
	void Shutdown();

private:

	void init_buffer(ID3D11Device *pD3D11Device);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);

	int m_VertexCount = 0;
	int m_IndexCount = 0;

    byhj::MatrixBuffer m_cbMatrix;
	d3d::Shader TestShader;

	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	ComPtr<ID3D11Buffer> m_pMVPBuffer;
};


}


#endif