#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include "common/d3dApp.h"
#include <common/d3dShader.h>
#include <common/d3dModel.h>

class D3DInitApp: public D3DApp
{
public:
	D3DInitApp()
	{
		m_AppName = L"DirectX11: ch04-Buffer-Shader";

		m_pInputLayout        = NULL;
		m_pVS                 = NULL;
		m_pPS                 = NULL;
		m_pSwapChain          = NULL;
		m_pD3D11Device        = NULL;
		m_pD3D11DeviceContext = NULL;
		m_pRenderTargetView   = NULL;
		m_pDepthStencilView   = NULL;
		m_pDepthStencilBuffer = NULL;
		m_pMVPBuffer          = NULL;
		m_pVertexBuffer       = NULL;
		m_pIndexBuffer        = NULL;
	}

	bool v_InitD3D();
	void v_Render();

	void v_Shutdown()
	{
		ReleaseCOM(m_pInputLayout       )
			ReleaseCOM(m_pVS                )
			ReleaseCOM(m_pPS                )
			ReleaseCOM(m_pSwapChain         )
			ReleaseCOM(m_pD3D11Device       )
			ReleaseCOM(m_pD3D11DeviceContext)
			ReleaseCOM(m_pRenderTargetView  )
			ReleaseCOM(m_pDepthStencilView  )
			ReleaseCOM(m_pMVPBuffer         )
			ReleaseCOM(m_pDepthStencilBuffer)
			ReleaseCOM(m_pVertexBuffer      )
			ReleaseCOM(m_pIndexBuffer       )
	}
private:
	bool init_buffer();
	bool init_device();
	bool init_shader();
	bool init_camera();

private:

	struct Vertex	//Overloaded Vertex Structure
	{
		Vertex(){}
		Vertex(float x, float y, float z,
			float cr, float cg, float cb, float ca)
			: pos(x,y,z), color(cr, cg, cb, ca){}

		XMFLOAT3 pos;
		XMFLOAT4 color;
	};
	struct MatrixBuffer
	{
		XMMATRIX  MVP;
	};
	MatrixBuffer cbMatrix;

	ID3D11InputLayout       *m_pInputLayout;
	ID3D11VertexShader      *m_pVS;
	ID3D11PixelShader       *m_pPS;
	IDXGISwapChain          *m_pSwapChain;
	ID3D11Device            *m_pD3D11Device;
	ID3D11DeviceContext     *m_pD3D11DeviceContext;
	ID3D11RenderTargetView  *m_pRenderTargetView;
	ID3D11DepthStencilView  *m_pDepthStencilView;
	ID3D11Texture2D         *m_pDepthStencilBuffer;
	ID3D11Buffer            *m_pVertexBuffer;
	ID3D11Buffer            *m_pMVPBuffer;
	ID3D11Buffer            *m_pIndexBuffer;
	int m_VertexCount;
	int m_IndexCount;

	Shader TestShader;

	XMMATRIX MVP;
	XMMATRIX Model;
	XMMATRIX View;
	XMMATRIX Proj;

	XMVECTOR camPos;
	XMVECTOR camTarget;
	XMVECTOR camUp;

	D3DModel ObjModel;
};

CALL_MAIN(D3DInitApp);

bool D3DInitApp::v_InitD3D()
{
	init_device();
	init_buffer();
	init_camera();
	init_shader();
	ObjModel.initModel(m_pD3D11Device, m_pD3D11DeviceContext, GetHwnd());
	ObjModel.loadModel("../../media/objects/spaceCompound.obj");

	return true;
}

void D3DInitApp::v_Render()
{
	//Render scene 

	D3DXCOLOR bgColor( 0.0f, 0.0f, 0.0f, 1.0f );
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView, bgColor);
	m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	static float rot = 0.0f;
	rot += .001f;
	if(rot > 6.26f)
		rot = 0.0f;

	//Define cube1's world space matrix
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Model  = XMMatrixTranslation( 0.0f, -10.0f, 30.0f );
	Model *= XMMatrixRotationAxis( rotaxis, rot);

	MVP = (Model * View * Proj);
	cbMatrix.MVP = XMMatrixTranspose(MVP);	
	ObjModel.Render(m_pD3D11DeviceContext, cbMatrix.MVP);
	m_pSwapChain->Present(0, 0);
}

bool D3DInitApp::init_device()
{
	HRESULT hr;

	//Create buffer desc
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
	bufferDesc.Width                   = m_ScreenWidth;
	bufferDesc.Height                  = m_ScreenHeight;
	bufferDesc.RefreshRate.Numerator   = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	//Create swapChain Desc
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

	//Create backbuffer, buffer also is a texture
	ID3D11Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);


	//Describe our Depth/Stencil Buffer
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
	m_pD3D11DeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );

	return true;
}

bool D3DInitApp::init_buffer()
{

	return true;
}

bool D3DInitApp::init_camera()
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
	camPos    = XMVectorSet( 0.0f, 3.0f, -8.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp     = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	View      = XMMatrixLookAtLH( camPos, camTarget, camUp );
	Proj      = XMMatrixPerspectiveFovLH( 0.4f*3.14f, GetAspect(), 1.0f, 1000.0f);

	return true;
}

bool D3DInitApp::init_shader()
{

	return true;
}