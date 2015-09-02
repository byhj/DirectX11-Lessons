#include "RenderSystem.h"

namespace byhj
{

void RenderSystem::v_Init()
{
	init_device();
	init_camera();
	init_object();

	return true;
}

void RenderSystem::v_Render()
{
	BeginScene();

	m_Camera.DetectInput(m_Timer.GetDeltaTime(), GetHwnd());
	m_Matrix.View  =   m_Camera.GetViewMatrix();
	m_Matrix.Proj  =  m_Proj;

	//////////////////////////////////////////////////////////////////
	XMMATRIX objectModel = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
	XMStoreFloat4x4(&m_Matrix.Model, XMMatrixTranspose(objectModel));
	ObjModel.Render(m_pD3D11DeviceContext, m_Matrix.Model, m_Matrix.View, m_Matrix.Proj);

	m_pD3D11DeviceContext->OMSetBlendState(0, 0, 0xffffffff);

	////////////////////////////////////////////////////////

	XMMATRIX sphereWorld = XMMatrixIdentity();
	m_Matrix.View._14 = 0.0f;
	m_Matrix.View._24 = 0.0f;
	m_Matrix.View._34 = 0.0f;
	m_Matrix.View._41 = 0.0f;
	m_Matrix.View._42 = 0.0f;
	m_Matrix.View._43 = 0.0f;
	XMStoreFloat4x4(&m_Matrix.Model, XMMatrixTranspose(sphereWorld));

	m_Skymap.Render(m_pD3D11DeviceContext, m_Matrix);

	///////////////////////////////////////////////////////

	DrawFps();

	EndScene();
}


void RenderSystem::v_Shutdown()
{
	ReleaseCOM(m_pSwapChain          );
	ReleaseCOM(m_pD3D11Device        );
	ReleaseCOM(m_pD3D11DeviceContext );
	ReleaseCOM(m_pRenderTargetView   );
	ReleaseCOM(m_pBlendState);
	ReleaseCOM(m_pRasterState);

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
	m_pD3D11Device->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView);

	//////////////////////Raterizer State/////////////////////////////
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;

	hr = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	m_pD3D11DeviceContext->RSSetState(m_pRasterState);

	///////////////////////////Blend state/////////////////////////////
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );
	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );
	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				 = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	m_pD3D11Device->CreateBlendState(&blendDesc, &m_pBlendState);
}

void RenderSystem::init_camera()
{
	//Viewport Infomation
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.Width    = m_ScreenWidth;
	vp.Height   = m_ScreenHeight;
	m_pD3D11DeviceContext->RSSetViewports(1, &vp);

	//MVP Matrix
	XMVECTOR camPos    = XMVectorSet(0.0f,  5.0f, -8.0f, 0.0f);
	XMVECTOR camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR camUp     = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	XMMATRIX View      = XMMatrixLookAtLH( camPos, camTarget, camUp );
	XMMATRIX Model     = XMMatrixIdentity();
	XMMATRIX Proj      = XMMatrixPerspectiveFovLH(0.4f*3.14f, GetAspect(), 1.0f, 1000.0f);

	XMStoreFloat4x4(&m_Proj, XMMatrixTranspose(Proj));
	XMStoreFloat4x4(&m_Model, XMMatrixTranspose(Model) );
	XMStoreFloat4x4(&m_View, XMMatrixTranspose(View) );

}



void RenderSystem::init_object()
{
	ObjModel.initModel(m_pD3D11Device, m_pD3D11DeviceContext, GetHwnd());
	ObjModel.loadModel("../../media/objects/spaceCompound.obj");

	m_Skymap.createSphere(m_pD3D11Device, 10, 10);
	m_Skymap.load_texture(m_pD3D11Device, L"../../media/textures/skymap.dds");
	m_Skymap.init_shader(m_pD3D11Device, GetHwnd());


	m_Font.Init(m_pD3D11Device);
	m_Timer.Init();
	m_Camera.Init(GetAppInst(), GetHwnd());

}

void RenderSystem::BeginScene()
{
	//Set status and Render scene 
	D3DXCOLOR bgColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView, bgColor);
	m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
	m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	m_pD3D11DeviceContext->RSSetState(m_pRasterState);
}

void RenderSystem::EndScene()
{
	m_pSwapChain->Present(0, 0);
}

void RenderSystem::DrawFps()
{
	static bool flag = true;
	if (flag)
	{
		m_Timer.Start();
		flag = false;
	}

	m_Timer.Count();
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;
	if (m_Timer.GetTotalTime() - timeElapsed >= 1.0f)
	{
		fps = frameCnt;
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

	m_Font.drawFps(m_pD3D11DeviceContext, (UINT)fps);
}

}