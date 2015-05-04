#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <xnamath.h>

IDXGISwapChain *pSwapChain = NULL;
ID3D11Device *pDevice = NULL;
ID3D11DeviceContext *pDeviceContext = NULL;
ID3D11RenderTargetView *pRenderTargetView =NULL;
ID3D11DepthStencilView *pDepthStencilView;
ID3D11Texture2D *pDepthStencilBuffer;
ID3D11Buffer* pMVPBuffer;
XMMATRIX MVP;
XMMATRIX cube1World;
XMMATRIX cube2World;
XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;
XMMATRIX View;
XMMATRIX Proj;
XMVECTOR camPos;
XMVECTOR camTarget;
XMVECTOR camUp;

ID3D11Buffer *pVB;
ID3D11Buffer *pIB;
ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;
ID3D10Blob *VS_Buffer;
ID3D10Blob *PS_Buffer;
ID3D11InputLayout *pInputLayout;

LPCTSTR WndClassName = "DirectX11-Indices";
HWND hWnd = NULL;
HRESULT hr;

const int Width = 800;
const int Height = 800;

struct cbPerObject
{
	XMMATRIX  MVP;
};
cbPerObject cbPerObj;

bool InitD3D(HINSTANCE hInstance);
void Clean();
bool InitScene();
void UpdateScene();
void RenderScene();
bool InitWindow(HINSTANCE hInstance, int ShowCmd, int width, int height, bool windowed);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, float r, float g, float b, float a)
		:Pos(x, y, z), Color(r, g, b, a) {}

	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{

	if (!InitWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, "Init Window Failed", "Error", MB_OK);
		return -1;
	}

	if (!InitD3D(hInstance))
	{
		MessageBox(0, "Init D3D Failed", "Error", MB_OK);
		return -1;
	}

	if (!InitScene())
	{
		MessageBox(0, "Init Scene Failed", "Error", MB_OK);
		return -1;
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			UpdateScene();
			RenderScene();
		}
	}

	Clean();
	return (int)msg.wParam;
}


bool InitD3D(HINSTANCE hInstance)
{
	HRESULT hr;

	//create buffer desc
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//create swapChain Desc
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, 
		&swapChainDesc, &pSwapChain, &pDevice,
		NULL, &pDeviceContext);

	//create backbuffer
	ID3D11Texture2D *backBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	hr = pDevice->CreateRenderTargetView(backBuffer, NULL, &pRenderTargetView);
	backBuffer->Release();

	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	return true;
}


bool InitScene()
{
	//vs, ps shader input layout
	HRESULT hr;
	hr = D3DX11CompileFromFile("quad.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile("quad.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
	hr = pDevice->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(),NULL, &pVS);
	hr = pDevice->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(),NULL, &pPS);

	pDeviceContext->VSSetShader(pVS, 0, 0);
	pDeviceContext->PSSetShader(pPS, 0, 0);

	//triangle data
	Vertex VertexData[] = 
	{
		Vertex( -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f ),
		Vertex( -1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f ),
		Vertex( +1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f ),
		Vertex( +1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f ),
		Vertex( -1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f ),
		Vertex( -1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f ),
		Vertex( +1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f ),
		Vertex( +1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f ),
	}; 

	DWORD ElementData[] =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D11_BUFFER_DESC iboDesc;
	ZeroMemory( &iboDesc, sizeof(iboDesc) );
	iboDesc.Usage = D3D11_USAGE_DEFAULT;
	iboDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	iboDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iboDesc.CPUAccessFlags = 0;
	iboDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = ElementData;
	pDevice->CreateBuffer(&iboDesc, &iinitData, &pIB);
	pDeviceContext->IASetIndexBuffer( pIB, DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC vboDesc;
	ZeroMemory(&vboDesc, sizeof(vboDesc));
	vboDesc.Usage = D3D11_USAGE_DEFAULT;
	vboDesc.ByteWidth = sizeof(Vertex) * 8;
	vboDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vboDesc.CPUAccessFlags = 0;
	vboDesc.MiscFlags = 0;

	//set data to vbo
	D3D11_SUBRESOURCE_DATA vbo;
	ZeroMemory(&vbo, sizeof(vbo));
	vbo.pSysMem = VertexData;
	hr = pDevice->CreateBuffer(&vboDesc, &vbo, &pVB);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);

	//set input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);
	pDevice->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &pInputLayout);
	pDeviceContext->IASetInputLayout(pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = Width;
	depthStencilDesc.Height    = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	pDevice->CreateTexture2D(&depthStencilDesc, NULL, &pDepthStencilBuffer);
	pDevice->CreateDepthStencilView(pDepthStencilBuffer, NULL, &pDepthStencilView);
	pDeviceContext->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );


	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));

	mvpDesc.Usage = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth = sizeof(XMMATRIX);
	mvpDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags = 0;
	hr = pDevice->CreateBuffer(&mvpDesc, NULL, &pMVPBuffer);
	//Camera information
	camPos = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	//Set the View matrix
	View = XMMatrixLookAtLH( camPos, camTarget, camUp );
	//Set the Projection matrix
	Proj = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)Width/Height, 1.0f, 1000.0f);

	//set viewpot
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = Width;
	vp.Height = Height;
	pDeviceContext->RSSetViewports(1, &vp);

	return true;
}

void UpdateScene()
{
	//Keep the cubes rotating
	rot += .0001f;
	if(rot > 6.28f)
		rot = 0.0f;

	//Reset cube1World
	cube1World = XMMatrixIdentity();
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Rotation = XMMatrixRotationAxis( rotaxis, rot);
	Translation = XMMatrixTranslation( 0.0f, 0.0f, 4.0f );
	cube1World = Translation * Rotation;

	//Reset cube2World
	cube2World = XMMatrixIdentity();
	Rotation = XMMatrixRotationAxis( rotaxis, -rot);
	Scale = XMMatrixScaling( 1.3f, 1.3f, 1.3f );
	cube2World = Rotation * Scale;

}
void Clean()
{
	pSwapChain->Release();
	pDevice->Release();
	pDeviceContext->Release();
	pVB->Release();
	pIB->Release();
	pVS->Release();
	pPS->Release();
	pMVPBuffer->Release();
	pDepthStencilView->Release();
	pDepthStencilBuffer->Release();
	pInputLayout->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
}

void RenderScene()
{
	D3DXCOLOR bgColor( 0.0, 0.0, 0.0, 1.0f );
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, bgColor);
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Set the World/View/Projection matrix, then send it to constant buffer in effect file
	MVP = cube1World * View * Proj;

	cbPerObj.MVP = XMMatrixTranspose(MVP);	
	pDeviceContext->UpdateSubresource(pMVPBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pDeviceContext->VSSetConstantBuffers( 0, 1, &pMVPBuffer);
	//Draw the first cube
	pDeviceContext->DrawIndexed( 36, 0, 0 );

	MVP = cube2World * View * Proj;
	cbPerObj.MVP = XMMatrixTranspose(MVP);	
	pDeviceContext->UpdateSubresource(pMVPBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pDeviceContext->VSSetConstantBuffers( 0, 1, &pMVPBuffer);
	pDeviceContext->DrawIndexed( 36, 0, 0 );

	pSwapChain->Present(0, 0);
}

bool InitWindow(HINSTANCE hInstance, int ShowCmd, int width, int height, bool windowed)
{

	WNDCLASSEX wc;	
	wc.cbSize = sizeof(WNDCLASSEX);	
	wc.style = CS_HREDRAW | CS_VREDRAW;	
	wc.lpfnWndProc = WndProc;	
	wc.cbClsExtra = NULL;	
	wc.cbWndExtra = NULL;	
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);	
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;	
	wc.lpszClassName = WndClassName;	
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 

	if (!RegisterClassEx(&wc))
	{

		MessageBox(NULL, "Registering Class Failded",	"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hWnd = CreateWindowEx(	
		NULL,	           
		WndClassName,	  
		"DirectX11-Init", 
		WS_OVERLAPPEDWINDOW,	
		300, 100,
		width,	
		height,	
		NULL,
		NULL,
		hInstance,	
		NULL
		);

	if (!hWnd )	
	{
		MessageBox(NULL, "Creating Window Failed", "Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hWnd, ShowCmd);
	UpdateWindow(hWnd);	

	return true;	
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(0, "Are you sure to exit?", "Really?",
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