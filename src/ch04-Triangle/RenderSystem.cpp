#include "RenderSystem.h"

namespace byhj
{


bool RenderSystem::v_InitD3D()
{
	init_device();
	init_camera();
	init_object();

	return true;
}

void RenderSystem::v_Render()
{

	BeginScene();

	m_Triangle.Render(m_pD3D11DeviceContext);

	EndScene();
}


void RenderSystem::v_Shutdown()
{
	ReleaseCOM(m_pSwapChain          );
	ReleaseCOM(m_pD3D11Device        );
	ReleaseCOM(m_pD3D11DeviceContext );
	ReleaseCOM(m_pRenderTargetView   );
}

void RenderSystem::init_device()
{
	HRESULT hr;

	/////////////////////////Create buffer desc///////////////////////
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width                   = m_ScreenWidth;
	bufferDesc.Height                  = m_ScreenHeight;
	bufferDesc.RefreshRate.Numerator   = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	//////////////////Create swapChain Desc////////////////////////////
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc         = bufferDesc;
	swapChainDesc.SampleDesc.Count   = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount        = 1;
	swapChainDesc.OutputWindow       = GetHwnd();
	swapChainDesc.Windowed           = TRUE;
	swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, 
		&swapChainDesc, &m_pSwapChain, &m_pD3D11Device,
		NULL, &m_pD3D11DeviceContext);

	//Create the rendertargetView, which buffer is a texture
	ID3D11Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	DebugHR(hr);
	pBackBuffer->Release();
}

void RenderSystem::init_camera()
{
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = m_ScreenWidth;
	vp.Height   = m_ScreenHeight;
	m_pD3D11DeviceContext->RSSetViewports(1, &vp);

}

void RenderSystem::init_object()
{
	m_Triangle.Init(m_pD3D11Device, GetHwnd());
}

void RenderSystem::BeginScene()
{
	//Set status and Render scene 
	D3DXCOLOR bgColor(0.2f, 0.3f, 0.4f, 1.0f);
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView, bgColor);
	m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

}

void RenderSystem::EndScene()
{
	m_pSwapChain->Present(0, 0);
}

}