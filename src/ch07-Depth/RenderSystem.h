#include "d3d/d3dApp.h"

#include "triangle.h"

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

	IDXGISwapChain          *m_pSwapChain;
	ID3D11Device            *m_pD3D11Device;
	ID3D11DeviceContext     *m_pD3D11DeviceContext;
	ID3D11RenderTargetView  *m_pRenderTargetView;
	ID3D11DepthStencilView  *m_pDepthStencilView;
	ID3D11Texture2D         *m_pDepthStencilBuffer;
};


}