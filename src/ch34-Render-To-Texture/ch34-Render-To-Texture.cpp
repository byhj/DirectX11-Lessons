#ifdef _DEBUG
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "d3d/d3dApp.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dFont.h"
#include "d3d/d3dTimer.h"
#include "d3d/d3dCubemap.h"
#include "d3d/d3dModel.h"
#include "d3d/d3dCamera.h"
#include "d3d/d3dRTT.h"

class D3DRenderSystem: public D3DApp
{
public:
	D3DRenderSystem()
	{
		m_AppName = L"DirectX11: ch04-Buffer-Shader";

		m_pSwapChain          = NULL;
		m_pD3D11Device        = NULL;
		m_pD3D11DeviceContext = NULL;
		m_pRenderTargetView   = NULL;
		m_pDepthStencilView   = NULL;
		m_pDepthStencilBuffer = NULL;

	}

	bool v_InitD3D();
	void v_Render();

	void v_Shutdown()
	{
			ReleaseCOM(m_pSwapChain         )
			ReleaseCOM(m_pD3D11Device       )
			ReleaseCOM(m_pD3D11DeviceContext)
			ReleaseCOM(m_pRenderTargetView  )
			ReleaseCOM(m_pDepthStencilView  )
			ReleaseCOM(m_pDepthStencilBuffer)

	}
private:
	void BeginScene();
	void EndScene();
	void init_object();
	bool init_device();
	bool init_camera();
	void init_rtt();
	void UpdateScene();

private:

	IDXGISwapChain           *m_pSwapChain;
	ID3D11Device             *m_pD3D11Device;
	ID3D11DeviceContext      *m_pD3D11DeviceContext;
	ID3D11RenderTargetView   *m_pRenderTargetView;
	ID3D11DepthStencilView   *m_pDepthStencilView;
	ID3D11Texture2D          *m_pDepthStencilBuffer;
	ID3D11RasterizerState    *m_pCCWcullMode;
	ID3D11RasterizerState    *m_pCWcullMode;

	ID3D11Texture2D          *pRttRenderTargetTexture;
	ID3D11RenderTargetView   *pRttRenderTargetView;
	ID3D11ShaderResourceView *pRttShaderResourceView;

	///////////////**************new**************////////////////////
	D3DSkymap skymap;
	D3DTimer timer;
	D3DFont font;
	D3DCamera camera;
	D3DModel ObjModel;
	D3DRTT d3dRtt;

	void DrawFps();
	void DrawMessage();

	XMFLOAT4X4 m_View, m_Model, m_Proj;
	XMMATRIX View, Model, Proj;
	int m_videoCardMemory;
	WCHAR m_videoCardInfo[255];
	float fps;
};

CALL_MAIN(D3DRenderSystem);

bool D3DRenderSystem::v_InitD3D()
{
	init_device();
	init_camera();
	init_object();
	init_rtt();

	return true;
}

void D3DRenderSystem::init_object()
{
	font.init(m_pD3D11Device);
	fps = 0.0f;
	timer.Reset();

	skymap.createSphere(m_pD3D11Device, 10, 10);
	skymap.load_texture(m_pD3D11Device, L"../../media/textures/skymap.dds");
	skymap.init_shader(m_pD3D11Device, GetHwnd());

	camera.InitDirectInput(GetAppInst(), GetHwnd());

	ObjModel.initModel(m_pD3D11Device, m_pD3D11DeviceContext, GetHwnd());
	ObjModel.loadModel("../../media/objects/ground.obj");

	d3dRtt.init_window(400.0f / m_ScreenWidth * 2.0f, 400.0f / m_ScreenHeight * 2.0f,
		200.0f * GetAspect() / m_ScreenWidth * 2.0f, 200.0f / m_ScreenHeight * 2.0f, GetAspect());
	d3dRtt.init_buffer(m_pD3D11Device, m_pD3D11DeviceContext);
	d3dRtt.init_shader(m_pD3D11Device, GetHwnd());
}

void D3DRenderSystem::init_rtt()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = m_ScreenWidth;
	textureDesc.Height = m_ScreenHeight;
	textureDesc.MipLevels  = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//Create the render target texture
	result = m_pD3D11Device->CreateTexture2D(&textureDesc, NULL, &pRttRenderTargetTexture);


	//Setup the description of the render target view
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = m_pD3D11Device->CreateRenderTargetView(pRttRenderTargetTexture, &renderTargetViewDesc, &pRttRenderTargetView);

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	result = m_pD3D11Device->CreateShaderResourceView(pRttRenderTargetTexture, &shaderResourceViewDesc, &pRttShaderResourceView);


};

void D3DRenderSystem::UpdateScene()
{
	
}

void D3DRenderSystem::v_Render()
{


	camera.DetectInput(timer.GetDeltaTime(), GetHwnd());

	//////////////////////////////////////Scene///////////////////////////////////
	XMMATRIX meshWorld = XMMatrixIdentity();
	XMMATRIX Rotation = XMMatrixRotationY(3.14f);
	XMMATRIX Scale = XMMatrixScaling( 1.0f, 1.0f, 1.0f );
	XMMATRIX Translation = XMMatrixTranslation( 0.0f, 0.0f, 0.0f );
	meshWorld = Rotation * Scale * Translation;

	///////////////////////Render to Texture//////////////////////////
	D3DXVECTOR4 bgColor = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pD3D11DeviceContext->OMSetRenderTargets(1, &pRttRenderTargetView, m_pDepthStencilView);
	m_pD3D11DeviceContext->ClearRenderTargetView(pRttRenderTargetView, bgColor);
	m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	meshWorld = XMMatrixTranspose(meshWorld);
	View      = XMMatrixTranspose(View);
	XMMATRIX tempProj = XMMatrixTranspose(Proj);
	XMStoreFloat4x4(&m_Model, meshWorld);
	XMStoreFloat4x4(&m_View, View);
	XMStoreFloat4x4(&m_Proj, tempProj);

	ObjModel.Render(m_pD3D11DeviceContext, m_Model, m_View, m_Proj);

	BeginScene();
	m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	ObjModel.Render(m_pD3D11DeviceContext, m_Model, m_View, m_Proj);
	//////////////////////////////////////SkyBox/////////////////////////////////////////
	m_View  = camera.GetViewMatrix();
	View = XMLoadFloat4x4(&m_View);

	XMMATRIX sphereWorld = XMMatrixIdentity();
	XMFLOAT4 camPosition = camera.GetCamPos();
	Scale       = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	Translation = XMMatrixTranslation(camPosition.x, camPosition.y, camPosition.z);
	sphereWorld = Scale * Translation;

	XMMATRIX MVP   = XMMatrixTranspose(sphereWorld * View * Proj); 
	XMFLOAT4X4 tempMVP;
	XMStoreFloat4x4(&tempMVP, MVP);
	skymap.Render(m_pD3D11DeviceContext, tempMVP);

	camera.DetectInput(timer.GetDeltaTime(), GetHwnd());

	///////////////////////////////////////////////////////////////////////////


	// Create an orthographic projection matrix for 2D rendering. 
	Model = XMMatrixIdentity();
	XMMATRIX tProj = XMMatrixOrthographicLH(m_ScreenWidth, m_ScreenHeight, 1.0f, 1000.0f);
	d3dRtt.Render(m_pD3D11DeviceContext, pRttShaderResourceView, Model, View, tProj);

	DrawMessage();

	UpdateScene();
   
	EndScene();
}

bool D3DRenderSystem::init_device()
{
	HRESULT hr;

	/////////////////////////Create buffer desc/////////////////////////////
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width                   = m_ScreenWidth;
	bufferDesc.Height                  = m_ScreenHeight;
	bufferDesc.RefreshRate.Numerator   = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	///////////////////////Create swapChain Desc/////////////////////////
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

	//Create the double buffer chain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, 
		&swapChainDesc, &m_pSwapChain, &m_pD3D11Device,
		NULL, &m_pD3D11DeviceContext);

	////////////////Create backbuffer, buffer also is a texture
	ID3D11Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);


	/////////////////////Describe our Depth/Stencil Buffer///////////////////////////
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

	m_pD3D11Device->CreateTexture2D(&depthStencilDesc, NULL, &m_pDepthStencilBuffer);
	m_pD3D11Device->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView);

	//////////////////////Raterizer State/////////////////////////////

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise =  true;
	hr = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_pCCWcullMode);
	DebugHR(hr);

	rasterDesc.FrontCounterClockwise = false;
	hr = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_pCWcullMode);
	DebugHR(hr);

	//////////////////////////Device Information//////////////////////////////
	// Create the state using the device.
	unsigned int numModes, i, numerator, denominator, stringLength;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGISurface *surface;
	DXGI_ADAPTER_DESC adapterDesc;

	// Create a DirectX graphics interface factory.
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	// Use the factory to create an adapter for the primary graphics interface (video card).
	factory->EnumAdapters(0, &adapter);
	adapter->GetDesc(&adapterDesc);
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	swprintf(m_videoCardInfo,L"Video Card  : %ls", adapterDesc.Description);

	return true;
}

bool D3DRenderSystem::init_camera()
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

	Proj  = XMMatrixPerspectiveFovLH( 0.4f*3.14f, GetAspect(), 1.0f, 1000.0f);

	return true;
}

void D3DRenderSystem::DrawFps()
{
	static bool flag = true;
	if (flag)
	{
		timer.Start();
		flag = false;
	}

	timer.Count();
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;
	if(timer.GetTotalTime() - timeElapsed >= 1.0f)
	{
		fps = frameCnt;
		frameCnt = 0;
		timeElapsed += 1.0f;
	}	

	font.drawFps(m_pD3D11DeviceContext, (UINT)fps);
}

void D3DRenderSystem::DrawMessage()
{
	WCHAR WinInfo[255];
	swprintf(WinInfo, L"Window Size: %d x %d", m_ScreenWidth, m_ScreenHeight);
	DrawFps();
	font.drawText(m_pD3D11DeviceContext, WinInfo, 22.0f, 10.0f, 40.0f, 0xff0099ff);
	font.drawText(m_pD3D11DeviceContext, m_videoCardInfo, 22.0f, 10.0f, 70.0f, 0xff0099ff);
}

void  D3DRenderSystem::BeginScene()
{
	D3DXVECTOR4 bgColor = D3DXVECTOR4(0.2f, 0.3f, 0.4f, 1.0f);

	//m_pD3D11DeviceContext->OMSetDepthStencilState(m_pDE, 1);
	m_pD3D11DeviceContext->OMSetBlendState(NULL, NULL, 0XFFFFFFFF);
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView, bgColor);
	m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_pD3D11DeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
	return;
}


void D3DRenderSystem::EndScene()
{
	m_pSwapChain->Present(0, 0);
}
