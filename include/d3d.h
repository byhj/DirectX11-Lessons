#ifndef D3D_H
#define D3D_H

#include "d3dUtil.h"

class D3D11App
{
public:
	D3D11App(HINSTANCE hInstance);
	virtual ~D3D11App();

	HINSTANCE GetInstance() const;
	HWND      GetHwnd()     const;
	float     GetAspect()   const;

public:
	int Run();
	bool InitScene();
	virtual void UpdateScene() {}
	virtual void RenderScene() {}
	virtual bool InitShader() { return true;}
	virtual bool InitBuffer() {return true;}
	virtual bool InitTexture() {return true;}
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//Mouse and Key 
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

protected:
	bool InitWindow();
	bool InitD3D();

protected:
	HINSTANCE hInstance;
	HWND hWnd;
	bool Paused;

	//object handle
	ID3D11Device              *pDevice;
	ID3D11DeviceContext       *pDeviceContext;
	IDXGISwapChain            *pSwapChain;
	ID3D11Texture2D           *pDepthStencilBuffer;
	ID3D11RenderTargetView    *pRenderTargetView;
	ID3D11DepthStencilView    *pDepthStencilView;
	D3D11_VIEWPORT             Viewport;

	std::wstring WindowTitle;
	D3D_DRIVER_TYPE  DriverType;
	int ClientWidth;
	int ClientHeight;
};



namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	D3D11App* pD3D11App = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return pD3D11App->WndProc(hwnd, msg, wParam, lParam);
}

//initial the d3d status
D3D11App::D3D11App(HINSTANCE hInstance)
	:	hInstance(hInstance),
	WindowTitle(L"D3D11 Application"),
	DriverType(D3D_DRIVER_TYPE_HARDWARE),
	ClientWidth(1000),
	ClientHeight(800),
	hWnd(0),
	Paused(false),

	pDevice(0),
	pDeviceContext(0),
	pSwapChain(0),
	pDepthStencilBuffer(0),
	pRenderTargetView(0),
	pDepthStencilView(0)
{
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
	pD3D11App = this;
}

D3D11App::~D3D11App()
{
	ReleaseCOM(pRenderTargetView);
	ReleaseCOM(pDepthStencilView);
	ReleaseCOM(pSwapChain);
	ReleaseCOM(pDepthStencilBuffer);

	// Restore all default settings.
	if( pDeviceContext )
		pDeviceContext->ClearState();

	ReleaseCOM(pDeviceContext);
	ReleaseCOM(pDevice);
}

HINSTANCE D3D11App::GetInstance() const
{
	return  hInstance;
}

HWND D3D11App::GetHwnd()const
{
	return hWnd;
}

float D3D11App::GetAspect()const
{
	return static_cast<float>(ClientWidth) / static_cast<float>(ClientHeight);
}

int D3D11App::Run()
{
	MSG msg = {0};

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
		{	
			UpdateScene();	
			RenderScene();
		}
	}

	return (int)msg.wParam;
}

bool D3D11App::InitScene()
{
	if(!InitWindow())
		return false;

	if(!InitD3D())
		return false;

	if(!InitBuffer())
		return false;

	if(!InitTexture())
		return false;

	if(!InitShader())
		return false;
	return true;
}


LRESULT CALLBACK D3D11App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(0, L"Are you sure to exit?", L"Really?",
				MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


bool D3D11App::InitWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	hWnd = CreateWindow(L"D3DWndClassName", WindowTitle.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0); 
	if( !hWnd )
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return true;
}

bool D3D11App::InitD3D()
{
	// Create the device and device context.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		DriverType,
		0,                 // no software device
		createDeviceFlags, 
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&pDevice,
		&featureLevel,
		&pDeviceContext);

	if( FAILED(hr) )
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if( featureLevel != D3D_FEATURE_LEVEL_11_0 )
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}


	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = ClientWidth;
	sd.BufferDesc.Height = ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(pDevice, &sd, &pSwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	assert(pDeviceContext);
	assert(pDevice);
	assert(pSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(pRenderTargetView);
	ReleaseCOM(pDepthStencilView);
	ReleaseCOM(pDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(pSwapChain->ResizeBuffers(1, ClientWidth, ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(pDevice->CreateRenderTargetView(backBuffer, 0, &pRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = ClientWidth;
	depthStencilDesc.Height    = ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	HR(pDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer));
	HR(pDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &pDepthStencilView));
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);


	// Set the viewport transform.
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width    = static_cast<float>(ClientWidth);
	Viewport.Height   = static_cast<float>(ClientHeight);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	pDeviceContext->RSSetViewports(1, &Viewport);

	return true;
}

#endif