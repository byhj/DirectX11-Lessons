#ifdef _DEBUG
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include "d3d/d3dApp.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dFont.h"
#include "d3d/d3dTimer.h"

#include <dinput.h>
class TextureApp: public D3DApp
{
public:
	TextureApp()
	{
		m_AppName = L"DirectX11: ch04-Buffer-Shader";
		m_pSwapChain          = NULL;
		m_pD3D11Device        = NULL;
		m_pD3D11DeviceContext = NULL;
		m_pRenderTargetView   = NULL;
		m_pDepthStencilView   = NULL;
		m_pDepthStencilBuffer = NULL;
		m_pMVPBuffer          = NULL;
		m_pVertexBuffer       = NULL;
		m_pIndexBuffer        = NULL;
		m_pTexture            = NULL;
		m_pTexSamplerState    = NULL;

		rot = 0.01f;
	    rotx = 0;
	    rotz = 0;
	    scaleX = 1.0f;
	    scaleY = 1.0f;
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
		ReleaseCOM(m_pMVPBuffer         )
		ReleaseCOM(m_pDepthStencilBuffer)
		ReleaseCOM(m_pVertexBuffer      )
		ReleaseCOM(m_pIndexBuffer       )
		ReleaseCOM(m_pTexture           )   
		ReleaseCOM(m_pTexSamplerState   ) 
	}
private:
	bool init_buffer();
	bool init_device();
	bool init_shader();
	bool init_camera();
	void init_texture(LPCWSTR texFile);
	void update();

private:

	struct Vertex	//Overloaded Vertex Structure
	{
		Vertex(){}
		Vertex(float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz)
			: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

		XMFLOAT3 pos;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
	};

	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;
	};
	MatrixBuffer cbMatrix;

	struct  LightBuffer
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;

		XMFLOAT3 lightDir;
		float    padding1;

		XMFLOAT3 lightPos;
		float    range;

		XMFLOAT3 att;
		float    padding2;
	};
	LightBuffer cbLight;

	IDirectInputDevice8* m_pDIKeyboard;
	IDirectInputDevice8* m_pDIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;
	bool InitDirectInput(HINSTANCE hInstance);
	void DetectInput(double time);

	float rotx ;
	float rotz ;
	float scaleX;
	float scaleY;
	XMMATRIX cube1World;
	XMMATRIX cube2World;
	XMMATRIX Rotationx;
	XMMATRIX Rotationz;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	float rot;

	IDXGISwapChain          *m_pSwapChain;
	ID3D11Device            *m_pD3D11Device;
	ID3D11DeviceContext     *m_pD3D11DeviceContext;
	ID3D11RenderTargetView  *m_pRenderTargetView;
	ID3D11DepthStencilView  *m_pDepthStencilView;
	ID3D11Texture2D         *m_pDepthStencilBuffer;
	ID3D11RasterizerState   *m_pRasterState;
	ID3D11Buffer            *m_pVertexBuffer;
	ID3D11Buffer            *m_pMVPBuffer;
	ID3D11Buffer            *m_pLightBuffer;
	ID3D11Buffer            *m_pIndexBuffer;
	ID3D11ShaderResourceView *m_pTexture;
	ID3D11SamplerState       *m_pTexSamplerState;
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

	D3DTimer timer;
	D3DFont font;
	float fps;
};

CALL_MAIN(TextureApp);

bool TextureApp::v_InitD3D()
{
	init_device();
	init_buffer();
	init_camera();
	init_shader();
	init_texture(L"../../media/textures/crate.bmp");
	font.init(m_pD3D11Device);
	fps = 0.0f;
	timer.Reset();

	if( !InitDirectInput(GetAppInst()) )
	{
		MessageBox(0, L"Direct Input Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}
	return true;
}
bool TextureApp::InitDirectInput(HINSTANCE hInstance)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL); 

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&m_pDIKeyboard,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&m_pDIMouse,
		NULL);

	hr = m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pDIKeyboard->SetCooperativeLevel(GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse);
	hr = m_pDIMouse->SetCooperativeLevel(GetHwnd(), DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void TextureApp::DetectInput(double time)
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	m_pDIKeyboard->Acquire();
	m_pDIMouse->Acquire();

	m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	m_pDIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

	if(keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(GetHwnd(), WM_DESTROY, 0, 0);

	if(keyboardState[DIK_LEFT] & 0x80)
	{
		rotz -= 1.0f * time;
	}
	if(keyboardState[DIK_RIGHT] & 0x80)
	{
		rotz += 1.0f * time;
	}
	if(keyboardState[DIK_UP] & 0x80)
	{
		rotx += 1.0f * time;
	}
	if(keyboardState[DIK_DOWN] & 0x80)
	{
		rotx -= 1.0f * time;
	}
	if(mouseCurrState.lX != mouseLastState.lX)
	{
		scaleX -= (mouseCurrState.lX * 0.001f);
	}
	if(mouseCurrState.lY != mouseLastState.lY)
	{
		scaleY -= (mouseCurrState.lY * 0.001f);
	}

	if ( rotx > 6.28 )
		rotx -=  6.28;
	else if ( rotx < 0 )
		rotx = 6.28 + rotx;

	if ( rotz > 6.28 )
		rotz -=  6.28;
	else if ( rotz < 0 )
		rotz =  6.28 + rotz;

	mouseLastState = mouseCurrState;

	return;
}

void TextureApp::v_Render()
{
	static bool flag = true;
	if (flag)
	{
		timer.Start();
		flag = false;
	}
	D3DXCOLOR bgColor( 0.0f, 0.0f, 0.0f, 1.0f );
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView, bgColor);
	m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);
	TestShader.use(m_pD3D11DeviceContext);
	// Set vertex buffer stride and offset.=
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex); 
	offset = 0;
	m_pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	static float rot = 0.0f;
	rot += .001f;
	if(rot > 6.26f)
		rot = 0.0f;

	m_pD3D11DeviceContext->PSSetShaderResources( 0, 1, &m_pTexture );
	m_pD3D11DeviceContext->PSSetSamplers( 0, 1, &m_pTexSamplerState );

	//////////////////////Define cube1's world space matrix//////////////////////
	XMVECTOR rotyaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotzaxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR rotxaxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	Rotation = XMMatrixRotationAxis(rotyaxis, rot);
	Rotationx = XMMatrixRotationAxis(rotxaxis, rotx);
	Rotationz = XMMatrixRotationAxis(rotzaxis, rotz);
	Translation = XMMatrixTranslation( 0.0f, 0.0f, 4.0f );

	//Set cube1's world space using the transformations
	cube1World = Translation * Rotation * Rotationx * Rotationz;

	cbMatrix.model = XMMatrixTranspose(cube1World);
	cbMatrix.view  = XMMatrixTranspose(View);
	cbMatrix.proj  = XMMatrixTranspose(Proj);

	m_pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	m_pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
    m_pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);


	//////////////////////Define cube2's world space matrix/////////////////////////////
	//Reset cube2World
	cube2World = XMMatrixIdentity();
	//Define cube2's world space matrix
	Rotation = XMMatrixRotationAxis( rotyaxis, -rot);
	///////////////**************new**************////////////////////
	Scale = XMMatrixScaling( scaleX, scaleY, 1.3f );
	///////////////**************new**************////////////////////
	//Set cube2's world space matrix
	cube2World = Rotation * Scale;

	cbMatrix.model = XMMatrixTranspose(cube2World);
	cbMatrix.view  = XMMatrixTranspose(View);
	cbMatrix.proj  = XMMatrixTranspose(Proj);

	m_pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	m_pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
	m_pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
	
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

	DetectInput(timer.GetDeltaTime());

	m_pSwapChain->Present(0, 0);
}

bool TextureApp::init_device()
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
	m_pD3D11DeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
	
	//////////////////////Raterizer State/////////////////////////////
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = false;
	hr = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	m_pD3D11DeviceContext->RSSetState(m_pRasterState);

	return true;
}

bool TextureApp::init_buffer()
{
	HRESULT hr;

	///////////////////////////Index Buffer ////////////////////////////////
	m_VertexCount = 24;
	Vertex VertexData[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,-1.0f,  -1.0f, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f,  0.0f, 0.0f,-1.0f,   1.0f, -1.0f),
		Vertex( 1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f,   1.0f, -1.0f),
		Vertex( 1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, -1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f,  1.0f, 1.0f,-1.0f,   -1.0f, 1.0f),
		Vertex( 1.0f, -1.0f, 1.0f,  0.0f, 1.0f, 1.0f,   -1.0f, 1.0f),
		Vertex( 1.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,    1.0f, 1.0f),
		Vertex(-1.0f,  1.0f, 1.0f,  1.0f, 0.0f,-1.0f,    1.0f, 1.0f),
								    				   
		// Top Face				    				   
		Vertex(-1.0f, 1.0f, -1.0f,  0.0f, 1.0f,-1.0f,   1.0f, -1.0f),
		Vertex(-1.0f, 1.0f,  1.0f,  0.0f, 0.0f,-1.0f,   1.0f,  1.0f),
		Vertex( 1.0f, 1.0f,  1.0f,  1.0f, 0.0f, 1.0f,   1.0f,  1.0f),
		Vertex( 1.0f, 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,   1.0f, -1.0f),
								    
		// Bottom Face								    
		Vertex(-1.0f, -1.0f, -1.0f,  1.0f, 1.0f,-1.0f,   -1.0f, -1.0f),
		Vertex( 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,   -1.0f, -1.0f),
		Vertex( 1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,   -1.0f,  1.0f),
		Vertex(-1.0f, -1.0f,  1.0f,  1.0f, 0.0f,-1.0f,   -1.0f,  1.0f),
									  				    
		// Left Face				  				    
		Vertex(-1.0f, -1.0f,  1.0f,  0.0f, 1.0f,-1.0f,   -1.0f,  1.0f),
		Vertex(-1.0f,  1.0f,  1.0f,  0.0f, 0.0f,-1.0f,    1.0f,  1.0f),
		Vertex(-1.0f,  1.0f, -1.0f,  1.0f, 0.0f,-1.0f,    1.0f, -1.0f),
		Vertex(-1.0f, -1.0f, -1.0f,  1.0f, 1.0f,-1.0f,   -1.0f, -1.0f),
									  				    
		// Right Face				  				    
		Vertex( 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,   -1.0f, -1.0f),
		Vertex( 1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,    1.0f, -1.0f),
		Vertex( 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,    1.0f,  1.0f),
		Vertex( 1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,   -1.0f,  1.0f),
	};												    

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC VertexBufferDesc;
	VertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	VertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags      = 0;
	VertexBufferDesc.MiscFlags           = 0;
	VertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA VBO;
	VBO.pSysMem          = VertexData;
	VBO.SysMemPitch      = 0;
	VBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = m_pD3D11Device->CreateBuffer(&VertexBufferDesc, &VBO, &m_pVertexBuffer);
    DebugHR(hr);

	/////////////////////////////////Index Buffer ///////////////////////////////////////
	unsigned int IndexData[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	m_IndexCount = 36;

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC IndexBufferDesc;
	IndexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth           = sizeof(unsigned int) * m_IndexCount;
	IndexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags      = 0;
	IndexBufferDesc.MiscFlags           = 0;
	IndexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem          = IndexData;
	IBO.SysMemPitch      = 0;
	IBO.SysMemSlicePitch = 0;

	hr = m_pD3D11Device->CreateBuffer(&IndexBufferDesc, &IBO, &m_pIndexBuffer);
    DebugHR(hr);

	////////////////////////////////Const Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	hr = m_pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);

	D3D11_BUFFER_DESC cbLightDesc;
	ZeroMemory(&cbLightDesc, sizeof(D3D11_BUFFER_DESC));
	cbLightDesc.Usage          = D3D11_USAGE_DYNAMIC;
	cbLightDesc.ByteWidth      = sizeof(LightBuffer);
	cbLightDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbLightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbLightDesc.MiscFlags      = 0;
	hr = m_pD3D11Device->CreateBuffer(&cbLightDesc, NULL, &m_pLightBuffer);
	DebugHR(hr);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = m_pD3D11DeviceContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DebugHR(hr);
	LightBuffer *plightData = (LightBuffer *)mappedResource.pData;

	plightData->ambient   = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	plightData->diffuse   = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	plightData->lightDir  = XMFLOAT3(0.25f, 0.5f, -1.0f);
	plightData->padding1  = 0.0f;
	plightData->lightPos  = XMFLOAT3(1.0f, 1.0f, 3.0f);
	plightData->range     = 100.0f;
	plightData->att       = XMFLOAT3(0.0f, 0.2f, 0.0f);
    plightData->padding2  = 0.0f;

	m_pD3D11DeviceContext->Unmap(m_pLightBuffer, 0);


	return true;
}

bool TextureApp::init_camera()
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

bool TextureApp::init_shader()
{
	HRESULT result;

	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[3];
	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "TEXCOORD";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	pInputLayoutDesc[2].SemanticName         = "NORMAL";
	pInputLayoutDesc[2].SemanticIndex        = 0;
	pInputLayoutDesc[2].Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc[2].InputSlot            = 0;
	pInputLayoutDesc[2].AlignedByteOffset    = 20;
	pInputLayoutDesc[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[2].InstanceDataStepRate = 0;
	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	TestShader.init(m_pD3D11Device, GetHwnd());
	TestShader.attachVS(L"triangle.vsh", pInputLayoutDesc, numElements);
	TestShader.attachPS(L"triangle.psh");
	TestShader.end();

	return true;
}

void TextureApp::init_texture(LPCWSTR texFile)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(m_pD3D11Device, texFile, NULL,NULL, &m_pTexture, NULL);
	DebugHR(hr);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = m_pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	DebugHR(hr);
}