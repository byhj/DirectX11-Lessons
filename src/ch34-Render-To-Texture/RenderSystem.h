#include "d3d/App.h"
#include "d3d/Font.h"
#include "d3d/Timer.h"
#include "d3d/Camera.h"
#include "d3d/d3dCubemap.h"
#include "d3d/d3dModel.h"
#include "d3d/d3dRTT.h"

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
	void init_fbo();

	void BeginScene();
	void EndScene();
	void DrawFps();

	float fps;

	D3DModel ObjModel;
d3d::Skymap m_Skymap;
	d3d::Font m_Font;
	d3d::Timer m_Timer;
	d3d::Camera m_Camera;
	byhj::MatrixBuffer m_Matrix;

	ID3D11Texture2D          *pRttRenderTargetTexture;
	ID3D11RenderTargetView   *pRttRenderTargetView;
	ID3D11ShaderResourceView *pRttShaderResourceView;
	D3DRTT d3dRtt;

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