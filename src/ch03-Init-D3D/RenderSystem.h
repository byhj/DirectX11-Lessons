#include "d3d/d3dApp.h"

#include <>
namespace byhj
{

class RenderSystem: public D3DApp
{
public:
	RenderSystem()
	{
		m_AppName = L"DirectX11: ch03-Init-D3D";
		m_pSwapChain          = 0;
		m_pD3D11Device        = 0;
		m_pD3D11DeviceContext = 0;
		m_pRenderTargetView   = 0;
	}
	~RenderSystem(){}
	
public:
	bool v_InitD3D();
	void v_Render();
	void v_Shutdown();

private:
	IDXGISwapChain          *m_pSwapChain;
	ID3D11Device            *m_pD3D11Device;
	ID3D11DeviceContext     *m_pD3D11DeviceContext;
	ID3D11RenderTargetView  *m_pRenderTargetView;
};


}