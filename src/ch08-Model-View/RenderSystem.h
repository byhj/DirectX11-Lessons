#include "d3d/d3dApp.h"

#include "triangle.h"
#include "Common.h"

namespace byhj
{

class RenderSystem: public D3DApp
{
public:
	RenderSystem()
	{
		m_AppName = L"DirectX11: ch07-Depth";
		m_pSwapChain          = nullptr;
		m_pD3D11Device        = nullptr;
		m_pD3D11DeviceContext = nullptr;
		m_pRenderTargetView   = nullptr;
		m_pDepthStencilView   = nullptr;
		m_pDepthStencilBuffer = nullptr;

		XMMATRIX Imat = XMMatrixIdentity();
	    XMStoreFloat4x4(&m_Model, Imat);
		XMStoreFloat4x4(&m_View,  Imat);
		XMStoreFloat4x4(&m_Proj,  Imat);
	}
	~RenderSystem(){}
	
public:
	bool v_InitD3D();
	void v_Render();
	void v_Shutdown();

private:

	void init_camera();
	void init_device();

	byhj::Triangle  m_Triangle;
	byhj::MatrixBuffer m_Matrix;
	XMFLOAT4X4 m_Model;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	IDXGISwapChain          *m_pSwapChain;
	ID3D11Device            *m_pD3D11Device;
	ID3D11DeviceContext     *m_pD3D11DeviceContext;
	ID3D11RenderTargetView  *m_pRenderTargetView;
	ID3D11DepthStencilView  *m_pDepthStencilView;
	ID3D11Texture2D         *m_pDepthStencilBuffer;
};


}