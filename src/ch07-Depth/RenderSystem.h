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

	}
	~RenderSystem(){}
	
public:
	bool v_InitD3D();
	void v_Render();
	void v_Shutdown();

private:

	void init_camera();
	void init_device();
	void init_object();
	void BeginScene();
	void EndScene();

	byhj::Triangle  m_Triangle;

	IDXGISwapChain          *m_pSwapChain          = nullptr;
	ID3D11Device            *m_pD3D11Device        = nullptr;
	ID3D11DeviceContext     *m_pD3D11DeviceContext = nullptr;
	ID3D11RenderTargetView  *m_pRenderTargetView   = nullptr;
	ID3D11DepthStencilView  *m_pDepthStencilView   = nullptr;
	ID3D11Texture2D         *m_pDepthStencilBuffer = nullptr;
};


}