#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
#include "d3d.h"
ID3D10Device1 *d3d101Device;	
IDXGIKeyedMutex *keyedMutex11;
IDXGIKeyedMutex *keyedMutex10;	
ID2D1RenderTarget *D2DRenderTarget;	
ID2D1SolidColorBrush *Brush;
ID3D11Texture2D *BackBuffer11;
ID3D11Texture2D *sharedTex11;	
ID3D11Buffer *d2dVertBuffer;
ID3D11Buffer *d2dIndexBuffer;
ID3D11ShaderResourceView *d2dTexture;
IDWriteFactory *DWriteFactory;
IDWriteTextFormat *TextFormat;
std::wstring printText;


struct Vertex	
{
	Vertex(){}
	Vertex(float x, float y, float z,
		   float u, float v)
		: Pos(x, y, z), TexCoord(u, v){}

	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
};

struct cbPerObject
{
	XMMATRIX  MVP;
};
cbPerObject cbPerObj;

const int Width = 800;
const int Height = 800;


class TextureApp : public D3D11App
{
public:
	TextureApp(HINSTANCE hInstance);
	~TextureApp();

	bool InitScene();
	bool InitBuffer();
	bool InitShader();
	bool InitStatus();
	bool InitTexture();
	void UpdateScene();
	void RenderScene();
	bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter);
	void InitD2DScreenTexture();
	void RenderText(std::wstring text);

private:
	ID3D11Buffer* pMVPBuffer;

	XMMATRIX MVP;
	XMMATRIX cube1World;
	XMMATRIX cube2World;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	float rot;
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
	LRESULT hr;
	ID3D11ShaderResourceView* pTexture;
	ID3D11SamplerState* pTexSamplerState;
};

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//init the window
	TextureApp app(hInstance);
	if ( !app.InitScene() )
		return 0;

	return  app.Run();
}


TextureApp::TextureApp(HINSTANCE hInstance)
	:D3D11App(hInstance) , rot(0.01f)
{
}

TextureApp::~TextureApp()
{
}

bool TextureApp::InitScene()
{
		InitD2DScreenTexture();
	if ( !D3D11App::InitScene() )
		return false;

	return true;
}

bool TextureApp::InitStatus()
{
	// Create DXGI factory to enumerate adapters///////////////////////////////////////////////////////////////////////////
	IDXGIFactory1 *DXGIFactory;

	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory);	

	// Use the first adapter	
	IDXGIAdapter1 *Adapter;

	hr = DXGIFactory->EnumAdapters1(0, &Adapter);

	DXGIFactory->Release();	

	//Create our Direct3D 11 Device and SwapChain//////////////////////////////////////////////////////////////////////////
	hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG |	D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL, NULL,	D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pDeviceContext);

	//Initialize Direct2D, Direct3D 10.1, DirectWrite
	InitD2D_D3D101_DWrite(Adapter);

	//Release the Adapter interface
	Adapter->Release();


}

///////////////**************new**************////////////////////
bool TextureApp::InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter)
{
	//Create our Direc3D 10.1 Device///////////////////////////////////////////////////////////////////////////////////////
	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL,D3D10_CREATE_DEVICE_DEBUG |	D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3d101Device	);	

	//Create Shared Texture that Direct3D 10.1 will render on//////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC sharedTexDesc;	

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = Width;
	sharedTexDesc.Height = Height;	
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;	
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;	
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;	

	hr = pDevice->CreateTexture2D(&sharedTexDesc, NULL, &sharedTex11);	

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);	

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;	

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);

	hr = sharedResource10->GetSharedHandle(&sharedHandle10);	

	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1///////////////////////////////////////////////////////////////////
	IDXGISurface1 *sharedSurface10;	

	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));

	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);	

	// Create D2D factory///////////////////////////////////////////////////////////////////////////////////////////////////
	ID2D1Factory *D2DFactory;	
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);	

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);	

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);

	sharedSurface10->Release();
	D2DFactory->Release();	

	// Create a solid color brush to draw something with		
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &Brush);

	//DirectWrite///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&DWriteFactory));

	hr = DWriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&TextFormat
		);

	hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);	
	return true;
}

void TextureApp::InitD2DScreenTexture()
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

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	pDevice->CreateBuffer(&indexBufferDesc, &iinitData, &d2dIndexBuffer);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = v;
	hr = pDevice->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);

	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	pDevice->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);

}
void TextureApp::RenderText(std::wstring text)
{
	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync(0);

	//Use D3D10.1 device
	keyedMutex10->AcquireSync(0, 5);			

	//Draw D2D content		
	D2DRenderTarget->BeginDraw();	

	//Clear D2D Background
	D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//Create our string
	std::wostringstream printString; 
	printString << text;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	Brush->SetColor(FontColor);	

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	//Draw the Text
	D2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		TextFormat,
		layoutRect,
		Brush
		);

	D2DRenderTarget->EndDraw();	

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync(1);

	//Use the D3D11 Device
	keyedMutex11->AcquireSync(1, 5);

	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	//Set the blend state for D2D render target texture objects
	pDeviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	pDeviceContext->IASetIndexBuffer( d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers( 0, 1, &d2dVertBuffer, &stride, &offset );

	MVP =  XMMatrixIdentity();
	cbPerObj.MVP = XMMatrixTranspose(MVP);	
	pDeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pDeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pDeviceContext->PSSetShaderResources( 0, 1, &d2dTexture );
	pDeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	pDeviceContext->RSSetState(CWcullMode);
	//Draw the second cube
	pDeviceContext->DrawIndexed( 6, 0, 0 );	
}
bool TextureApp::InitBuffer()
{
	Vertex VertexData[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
		Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
		Vertex( 1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
		Vertex( 1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
		Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex( 1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

		// Left Face
		Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Right Face
		Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
		Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
	};

	DWORD ElementData[] = {
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
	vboDesc.ByteWidth = sizeof(Vertex) * 24;
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

	return true;
}

bool TextureApp::InitTexture()
{
	hr = D3DX11CreateShaderResourceViewFromFile( pDevice, L"../media/texture/byhj.jpg",
		NULL, NULL, &pTexture, NULL );

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = pDevice->CreateSamplerState( &sampDesc, &pTexSamplerState );

	return true;
}
void TextureApp::UpdateScene()
{
	//Keep the cubes rotating
	rot += .001f;
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

bool TextureApp::InitShader()
{

	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
	hr = pDevice->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(),NULL, &pVS);
	hr = pDevice->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(),NULL, &pPS);

	pDeviceContext->VSSetShader(pVS, 0, 0);
	pDeviceContext->PSSetShader(pPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	};

	UINT numElements = ARRAYSIZE(layout);
	pDevice->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &pInputLayout);
	pDeviceContext->IASetInputLayout(pInputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void TextureApp::RenderScene()
{
	assert(pDeviceContext);
	assert(pSwapChain);
	XMFLOAT4 bgColor(0.0f, 0.0f, 0.0f, 1.0f);
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, reinterpret_cast<const float*>(&bgColor));
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	//Set the cubes index buffer
	pDeviceContext->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the cubes vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers( 0, 1, &squareVertBuffer, &stride, &offset );

	//Set the World/View/Projection matrix, then send it to constant buffer in effect file
	MVP = cube1World * View * Proj;

	cbPerObj.MVP = XMMatrixTranspose(MVP);	
	pDeviceContext->UpdateSubresource(pMVPBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pDeviceContext->VSSetConstantBuffers( 0, 1, &pMVPBuffer);
	pDeviceContext->PSSetShaderResources( 0, 1, &pTexture );
	pDeviceContext->PSSetSamplers( 0, 1, &pTexSamplerState );
	pDeviceContext->DrawIndexed( 36, 0, 0 );

	MVP = cube2World * View * Proj;
	cbPerObj.MVP = XMMatrixTranspose(MVP);	
	pDeviceContext->UpdateSubresource(pMVPBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pDeviceContext->VSSetConstantBuffers( 0, 1, &pMVPBuffer);
	pDeviceContext->PSSetShaderResources( 0, 1, &pTexture );
	pDeviceContext->PSSetSamplers( 0, 1, &pTexSamplerState );
	pDeviceContext->DrawIndexed( 36, 0, 0 );

	RenderText(L"Hello World");

	HR(pSwapChain->Present(0, 0));
}