#include "d3d/App.h"

#include "triangle.h"

namespace byhj
{

class RenderSystem: public d3d::App
{
public:
	RenderSystem()
	{
		m_AppName = L"DirectX11: ch03-Init-D3D";
	}
	~RenderSystem(){}
	
public:
	void v_Init() override;
	void v_Update() override;
	void v_Render() override;
	void v_Shutdown() override;

private:

	void init_device();
	void init_camera();
	void init_object();

	void BeginScene();
	void EndScene();

	byhj::Triangle  m_Triangle;

	ComPtr<IDXGISwapChain>          m_pSwapChain;
	ComPtr<ID3D11Device>            m_pD3D11Device;
	ComPtr<ID3D11DeviceContext>     m_pD3D11DeviceContext;
	ComPtr<ID3D11RenderTargetView>  m_pRenderTargetView;
};


}