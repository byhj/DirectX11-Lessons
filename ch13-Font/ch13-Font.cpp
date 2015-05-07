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

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
};

cbPerObject cbPerObj;

const int Width = 800;
const int Height = 800;

//Fps Data and Function
double countsPerSecond = 0.0;
__int64 CounterStart = 0;
int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;
void StartTimer();
double GetTime();
double GetFrameTime();

class TextureApp : public D3D11App
{
public:
	TextureApp(HINSTANCE hInstance);
	~TextureApp();

	bool InitScene();
	bool InitBuffer();
	bool InitShader();
	bool InitD3D();
	bool InitStatus();
	bool InitTexture();
	int	 Run();
	void UpdateScene();
	void RenderScene();
	void RenderText(std::wstring text);

private:
	ID3D11Buffer          *pCubeIndexBuffer;
	ID3D11Buffer          *pCubeVertBuffer;
	ID3D11VertexShader    *pVS;
	ID3D11PixelShader     *pPS;
	ID3D10Blob            *pVS_Buffer;
	ID3D10Blob            *pPS_Buffer;
	ID3D11InputLayout     *pInputLayout;
	ID3D11Buffer          *cbPerObjectBuffer;

	//Matrix
	XMMATRIX WVP;
	XMMATRIX cube1World;
	XMMATRIX cube2World;
	XMMATRIX camView;
	XMMATRIX camProjection;

	XMMATRIX d2dWorld;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;

	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	float rot;
	HRESULT hr;
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

int TextureApp::Run()
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

TextureApp::TextureApp(HINSTANCE hInstance)
	:D3D11App(hInstance) , rot(0.01f)
{
}

TextureApp::~TextureApp()
{
}
void TextureApp::RenderText(std::wstring text)
{
	pkeyedMutex11->ReleaseSync(0);
	pkeyedMutex10->AcquireSync(0, 5);			

	pD2DRenderTarget->BeginDraw();	
	pD2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//Create our string
	std::wostringstream printString; 
	printString << text;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	pBrush->SetColor(FontColor);	

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	//Draw the Text
	pD2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		pTextFormat,
		layoutRect,
		pBrush
		);

	pD2DRenderTarget->EndDraw();	

	pkeyedMutex10->ReleaseSync(1);
	pkeyedMutex11->AcquireSync(1, 5);

	pD3D11DeviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	pD3D11DeviceContext->IASetIndexBuffer(pD2DIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &pD2DVertBuffer, &stride, &offset );

	WVP =  XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	pD3D11DeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, &pD2DTexture );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	pD3D11DeviceContext->RSSetState(CWcullMode);
	//Draw the second cube
	pD3D11DeviceContext->DrawIndexed( 6, 0, 0 );	
}

bool TextureApp::InitScene()
{
	if ( !D3D11App::InitScene() )
		return false;

	return true;
}

bool TextureApp::InitBuffer()
{
	//Create the vertex buffer
	Vertex v[] =
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

	DWORD indices[] = {
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

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth      = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	pD3D11Device->CreateBuffer(&indexBufferDesc, &iinitData, &pCubeIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth      = sizeof( Vertex ) * 24;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 
	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = v;
	hr = pD3D11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &pCubeVertBuffer);

	return true;
}

bool TextureApp::InitTexture()
{
	return true;
}

bool TextureApp::InitStatus()
{
	InitD2DScreenTexture();

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width    = Width;
	viewport.Height   = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	pD3D11DeviceContext->RSSetViewports(1, &viewport);

	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage          = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth      = sizeof(cbPerObject);
	cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags      = 0;

	hr = pD3D11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	camPosition = XMVectorSet( 0.0f, 3.0f, -8.0f, 0.0f );
	camTarget   = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp       = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)Width/Height, 1.0f, 1000.0f);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );

	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	HR( D3DX11CreateShaderResourceViewFromFile( pD3D11Device, L"../common/media/texture/byhj.jpg",
		NULL, NULL, &CubesTexture, NULL ) );

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD         = 0;
	sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = pD3D11Device->CreateSamplerState( &sampDesc, &CubesTexSamplerState );

	pD3D11Device->CreateBlendState(&blendDesc, &Transparency);

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = pD3D11Device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;

	hr = pD3D11Device->CreateRasterizerState(&cmdesc, &CWcullMode);

	return true;
}


void TextureApp::UpdateScene()
{
	//Keep the cubes rotating
	rot += 0.001f;
	if(rot > 6.28f)
		rot = 0.0f;

	//Reset cube1World
	cube1World = XMMatrixIdentity();

	//Define cube1's world space matrix
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Rotation = XMMatrixRotationAxis( rotaxis, rot);
	Translation = XMMatrixTranslation( 0.0f, 0.0f, 4.0f );

	//Set cube1's world space using the transformations
	cube1World = Translation * Rotation;

	//Reset cube2World
	cube2World = XMMatrixIdentity();

	//Define cube2's world space matrix
	Rotation = XMMatrixRotationAxis( rotaxis, -rot);
	Scale = XMMatrixScaling( 1.3f, 1.3f, 1.3f );

	//Set cube2's world space matrix
	cube2World = Rotation * Scale;
}


bool TextureApp::InitShader()
{
	//Compile Shaders from shader file
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &pVS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &pPS_Buffer, 0, 0);

	//Create the Shader Objects
	hr = pD3D11Device->CreateVertexShader(pVS_Buffer->GetBufferPointer(), pVS_Buffer->GetBufferSize(), NULL, &pVS);
	hr = pD3D11Device->CreatePixelShader(pPS_Buffer->GetBufferPointer(), pPS_Buffer->GetBufferSize(), NULL, &pPS);

	//Set Vertex and Pixel Shaders
	pD3D11DeviceContext->VSSetShader(pVS, 0, 0);
	pD3D11DeviceContext->PSSetShader(pPS, 0, 0);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	};

	UINT numElements = ARRAYSIZE(layout);
	hr = pD3D11Device->CreateInputLayout( layout, numElements, pVS_Buffer->GetBufferPointer(), 
		pVS_Buffer->GetBufferSize(), &pInputLayout );

	pD3D11DeviceContext->IASetInputLayout( pInputLayout );
	pD3D11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	return true;
}

void TextureApp::RenderScene()
{

	float bgColor[4] = {(0.0f, 0.0f, 0.0f, 0.0f)};
	pD3D11DeviceContext->ClearRenderTargetView(pRenderTargetView, bgColor);	
	pD3D11DeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	pD3D11DeviceContext->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );
	pD3D11DeviceContext->OMSetBlendState(0, 0, 0xffffffff);

	pD3D11DeviceContext->IASetIndexBuffer(pCubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &pCubeVertBuffer, &stride, &offset );


	WVP = cube1World * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	pD3D11DeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, &CubesTexture );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );
	pD3D11DeviceContext->RSSetState(CWcullMode);
	pD3D11DeviceContext->DrawIndexed( 36, 0, 0 );

	WVP = cube2World * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	pD3D11DeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, &CubesTexture );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );
	pD3D11DeviceContext->RSSetState(CWcullMode);
	pD3D11DeviceContext->DrawIndexed( 36, 0, 0 );

	RenderText(L"Hello World");

	//Present the backbuffer to the screen
	pSwapChain->Present(0, 0);
}