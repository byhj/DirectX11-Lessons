#include "RenderSystem.h"

namespace byhj
{

void RenderSystem::v_Init()
{
	init_device();
	init_camera();
	init_object();
}
void RenderSystem::v_Update()
{

}
void RenderSystem::v_Render()
{
	BeginScene();

	FLOAT currTime = GetTickCount64();
	FLOAT rot = currTime/1000.0f;

	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX Model = XMMatrixRotationAxis(rotaxis, rot);
	XMStoreFloat4x4(&m_Model, XMMatrixTranspose(Model) );

	m_Matrix.Model = m_Model;
	m_Matrix.View  = m_View;
	m_Matrix.Proj  = m_Proj;
	m_Cube.Render(m_pD3D11DeviceContext.Get(), m_Matrix);

	EndScene();
}


void RenderSystem::v_Shutdown()
{
	m_Cube.Shutdown();
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
	//DebugHR(hr);
	pBackBuffer->Release();


	///////////////////////////////Describe our Depth/Stencil Buffer////////////////////////
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width              = m_ScreenWidth;
	depthStencilDesc.Height             = m_ScreenHeight;
	depthStencilDesc.MipLevels          = 1;
	depthStencilDesc.ArraySize          = 1;
	depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags     = 0; 
	depthStencilDesc.MiscFlags          = 0;

	//Same as color buffer, depthStencil use renderTarget view to make the buffer is a texture
	m_pD3D11Device->CreateTexture2D(&depthStencilDesc, NULL, &m_pDepthStencilBuffer);
	m_pD3D11Device->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), NULL, &m_pDepthStencilView);


	//////////////////////Raterizer State/////////////////////////////
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;  //We use the wireframe mode now
	rasterDesc.CullMode = D3D11_CULL_NONE;       //We not use cull face now
	rasterDesc.FrontCounterClockwise = false;
	hr = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_pRasterState);

}

void RenderSystem::init_camera()
{
	//Viewport Infomation
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = m_ScreenWidth;
	vp.Height   = m_ScreenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pD3D11DeviceContext->RSSetViewports(1, &vp);

	//MVP Matrix
	XMVECTOR camPos    = XMVectorSet( 0.0f, 0.0f, -5.0f, 0.0f );
	XMVECTOR camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR camUp     = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	XMMATRIX View      = XMMatrixLookAtLH( camPos, camTarget, camUp );
	XMMATRIX Proj      = XMMatrixPerspectiveFovLH( 0.4f*3.14f, GetAspect(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_View, XMMatrixTranspose(View) );
	XMStoreFloat4x4(&m_Proj, XMMatrixTranspose(Proj) );
}

void RenderSystem::init_object()
{
	m_Cube.Init(m_pD3D11Device.Get(), GetHwnd());
}

void RenderSystem::BeginScene()
{
	//Set status and Render scene 
	float bgColor[] = {0.2f, 0.3f, 0.4f, 1.0f};
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), bgColor);
	m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
	m_pD3D11DeviceContext->RSSetState(m_pRasterState.Get());
	m_pD3D11DeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());


}

void RenderSystem::EndScene()
{
	m_pSwapChain->Present(0, 0);
}\
}