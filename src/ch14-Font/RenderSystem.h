#include "d3d/App.h"
#include "d3d/Font.h"

#include "Cube.h"
#include "Common.h"

namespace byhj
{

class RenderSystem: public d3d::App
{
public:
	RenderSystem()
	{
		m_AppName = L"DirectX11: ch07-Depth";

		XMMATRIX Imat = XMMatrixIdentity();
	    XMStoreFloat4x4(&m_Model, Imat);
		XMStoreFloat4x4(&m_View,  Imat);
		XMStoreFloat4x4(&m_Proj,  Imat);
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
	void BeginScene();
	void EndScene();

	byhj::Cube  m_Cube;
	d3d::Font m_Font;
	byhj::MatrixBuffer m_Matrix;
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