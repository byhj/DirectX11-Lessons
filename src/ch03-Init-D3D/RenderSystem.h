#include "d3d/d3dApp.h"

namespace byhj
{

class RenderSystem: public D3DApp
{
public:
	RenderSystem()
	{
		m_AppName = L"DirectX11: ch03-Init-D3D";
	}
	~RenderSystem(){}
	
public:
	bool v_InitD3D();
	void v_Render();
	void v_Shutdown();

private:
	void BeginScene();
	void EndScene();

	IDXGISwapChain          *m_pSwapChain         = nullptr;
	ID3D11Device            *m_pD3D11Device       = nullptr;
	ID3D11DeviceContext     *m_pD3D11DeviceContext= nullptr;
	ID3D11RenderTargetView  *m_pRenderTargetView  = nullptr;
};


}