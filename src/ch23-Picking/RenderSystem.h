#include "d3d/App.h"
#include "d3d/Font.h"
#include "d3d/Timer.h"
#include "d3d/Camera.h"
#include "Cubemap.h"
#include "d3d/Model.h"

#include "Common.h"

#include"d3dPicking.h"

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

		score = 0;
		memset(bottleHit, 0, sizeof(bottleHit));
		fps = 0.0f;
	}
	~RenderSystem(){}
	
public:
	void v_Init() override;
	void v_Update() override;
	void v_Render() override;
	void v_Shutdown() override;

private:

	void init_camera();
	void init_device();
	void init_object();
	void CheckPick();

	void BeginScene();
	void EndScene();
	void DrawFps();

	float fps;

	XMMATRIX bottleModel[20];
	int bottleHit[20];
	int score;

	d3d::Model ObjModel;
	d3d::Model BottomModel;
    byhj::Skymap m_Skymap;
	d3d::Font m_Font;
	d3d::Timer m_Timer;
	d3d::Camera m_Camera;
	byhj::MatrixBuffer m_Matrix;

	D3DPicking d3dPicking;

	XMFLOAT4X4 m_Model;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11Device> m_pD3D11Device;
	ComPtr<ID3D11DeviceContext> m_pD3D11DeviceContext;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11BlendState> m_pBlendState;
	ComPtr<ID3D11RasterizerState> m_pRasterState;
};


}