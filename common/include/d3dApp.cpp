#include "d3dApp.h"

struct Vertex	
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v)
		: Pos(x, y, z), TexCoord(u, v){}

	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
};

namespace
{
	D3D11App* pD3D11App = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

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

	pD3D101Device(0),
	pD3D11DeviceContext(0),
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
	if( pD3D11DeviceContext )
		pD3D11DeviceContext->ClearState();

	ReleaseCOM(pD3D11DeviceContext);
	ReleaseCOM(pD3D11Device);
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

	InitD2DScreenTexture();

	if(!InitShader())
		return false;

	if(!InitBuffer())
		return false;

	if(!InitStatus())
		return false;

	if(!InitTexture())
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
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
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
	//Describe our SwapChain Buffer
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width                   = ClientWidth;
	bufferDesc.Height                  = ClientHeight;
	bufferDesc.RefreshRate.Numerator   = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc; 
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc         = bufferDesc;
	swapChainDesc.SampleDesc.Count   = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount        = 1;
	swapChainDesc.OutputWindow       = hWnd; 
	swapChainDesc.Windowed           = TRUE; 
	swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

	// Create DXGI factory to enumerate adapters
	IDXGIFactory1 *DXGIFactory;
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory);	
	
	IDXGIAdapter1 *Adapter;
	hr = DXGIFactory->EnumAdapters1(0, &Adapter);
	
	DXGIFactory->Release();	

	//Create our Direct3D 11 Device and SwapChain//////////////////////////////////////////////////////////////////////////
	hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG |	D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL, NULL,	D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pD3D11Device, NULL, &pD3D11DeviceContext);
	

	//Initialize Direct2D, Direct3D 10.1, DirectWrite
	InitD2D_D3D101_DWrite(Adapter);
	Adapter->Release();

	//Create our BackBuffer and Render Target
	hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&pBackBuffer11 );
	
	hr = pD3D11Device->CreateRenderTargetView( pBackBuffer11, NULL, &pRenderTargetView );
	

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width              = ClientWidth;
	depthStencilDesc.Height             = ClientHeight;
	depthStencilDesc.MipLevels          = 1;
	depthStencilDesc.ArraySize          = 1;
	depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags     = 0; 
	depthStencilDesc.MiscFlags          = 0;

	//Create the Depth/Stencil View
	hr = pD3D11Device->CreateTexture2D(&depthStencilDesc, NULL, &pDepthStencilBuffer);
	hr = pD3D11Device->CreateDepthStencilView(pDepthStencilBuffer, NULL, &pDepthStencilView);
	

	return true;
}


bool D3D11App::InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter)
{
	//Create our Direc3D 10.1 Device///////////////////////////////////////////////////////////////////////////////////////
	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL,D3D10_CREATE_DEVICE_DEBUG |	D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &pD3D101Device);
	

	//Create Shared Texture that Direct3D 10.1 will render on//////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC sharedTexDesc;	
	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));
	sharedTexDesc.Width            = ClientWidth;
	sharedTexDesc.Height           = ClientHeight;	
	sharedTexDesc.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels        = 1;	
	sharedTexDesc.ArraySize        = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage            = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags        = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;	
	sharedTexDesc.MiscFlags        = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;	

	hr = pD3D11Device->CreateTexture2D(&sharedTexDesc, NULL, &pSharedTex11);	
    
	hr = pSharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&pkeyedMutex11);	
	

	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;	
	hr = pSharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);
	
	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1///////////////////////////////////////////////////////////////////
	IDXGISurface1 *sharedSurface10;	

	hr = pD3D101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));
	
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&pkeyedMutex10);	
    

	// Create D2D factory///////////////////////////////////////////////////////////////////////////////////////////////////
	ID2D1Factory *D2DFactory;	
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);	
	
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);	

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &pD2DRenderTarget);
	
	sharedSurface10->Release();
	D2DFactory->Release();	

	// Create a solid color brush to draw something with		
	hr = pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &pBrush);
	
	//DirectWrite///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory));
	
	hr = pDWriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&pTextFormat
		);
	
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	pD3D101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);	

	return true;
}


void D3D11App::InitD2DScreenTexture()
{
	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	};

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth      = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	pD3D11Device->CreateBuffer(&indexBufferDesc, &iinitData, &pD2DIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth      = sizeof( Vertex ) * 4;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = v;
	hr = pD3D11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &pD2DVertBuffer);
	

	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	pD3D11Device->CreateShaderResourceView(pSharedTex11, NULL, &pD2DTexture);

}

void D3D11App::RenderText(std::wstring text)
{

}
