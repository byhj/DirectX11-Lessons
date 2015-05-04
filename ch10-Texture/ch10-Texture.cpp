#include "d3d.h"

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
	bool InitTexture();
	void UpdateScene();
	void RenderScene();

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
	if ( !D3D11App::InitScene() )
		return false;

	return true;
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

	HR(pSwapChain->Present(0, 0));
}