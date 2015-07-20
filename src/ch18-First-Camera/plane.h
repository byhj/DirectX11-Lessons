#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <d3d11.h>
#include <xnamath.h>

#include "Common.h"
#include "d3d/d3dShader.h"

namespace byhj
{

class Plane
{

public:
	Plane() 
	{
		m_VertexCount = 0;
		m_IndexCount  = 0;
		m_pVertexBuffer = nullptr;
		m_pIndexBuffer  = nullptr;
		m_pTexture         = nullptr;
		m_pTexSamplerState = nullptr;
	}
	~Plane() {}

public:
	void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const byhj::MatrixBuffer &matrix);
	void Shutdown();

private:

	void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

	int m_VertexCount;
	int m_IndexCount;
    byhj::MatrixBuffer m_cbMatrix;
	byhj::LightBuffer cbLight;
	Shader TestShader;

	ID3D11Buffer             *m_pVertexBuffer;
	ID3D11Buffer             *m_pIndexBuffer;
	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11Buffer             *m_pLightBuffer;
	ID3D11ShaderResourceView *m_pTexture;
	ID3D11SamplerState       *m_pTexSamplerState;
};


}


#endif