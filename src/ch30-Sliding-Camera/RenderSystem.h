#include "d3d/App.h"
#include "d3d/d3dFont.h"
#include "d3d/d3dTimer.h"
#include "d3d/d3dCamera.h"

#include "Plane.h"
#include "Common.h"

namespace byhj
{

class RenderSystem: public d3d::App
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
		m_pBlendState         = nullptr;
		m_pRasterState        = nullptr;

		XMMATRIX Imat = XMMatrixIdentity();
	    XMStoreFloat4x4(&m_Model, Imat);
		XMStoreFloat4x4(&m_View,  Imat);
		XMStoreFloat4x4(&m_Proj,  Imat);

		fps = 0.0f;
	}
	~RenderSystem(){}
	
public:
	void v_Init();
	void v_Render();
	void v_Shutdown();

private:

	void init_camera();
	void init_device();
	void init_object();
	void BeginScene();
	void EndScene();
	void DrawFps();

	float fps;

	byhj::Plane  m_Plane;
	byhj::D3DFont m_Font;
	byhj::D3DTimer m_Timer;
	byhj::D3DCamera m_Camera;
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
	ID3D11BlendState        *m_pBlendState;
	ID3D11RasterizerState   *m_pRasterState;
};


}