#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include "d3d/d3dApp.h"
#include <d3d/d3dShader.h>

class BlendApp: public D3DApp
{
public:
	BlendApp()
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
private:

	struct Vertex	//Overloaded Vertex Structure
	{
		Vertex(){}
		Vertex(float x, float y, float z,
			float u, float v)
			: pos(x,y,z), texCoord(u, v){}

		XMFLOAT3 pos;
		XMFLOAT2 texCoord;
	};

	struct MatrixBuffer
	{
		XMFLOAT4X4  MVP;
	};
	MatrixBuffer cbMatrix;

	IDXGISwapChain           *m_pSwapChain;
	ID3D11Device             *m_pD3D11Device;
	ID3D11DeviceContext      *m_pD3D11DeviceContext;
	ID3D11RenderTargetView   *m_pRenderTargetView;
	ID3D11DepthStencilView   *m_pDepthStencilView;
	ID3D11Texture2D          *m_pDepthStencilBuffer;
	ID3D11RasterizerState    *m_pRasterState;
	ID3D11Buffer             *m_pVertexBuffer;
	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11Buffer             *m_pIndexBuffer;
	ID3D11ShaderResourceView *m_pTexture;
	ID3D11SamplerState       *m_pTexSamplerState;
	ID3D11BlendState         *m_pBlendState;


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
};

CALL_MAIN(BlendApp);

bool BlendApp::v_InitD3D()
{
	init_device();
	init_buffer();
	init_camera();
	init_shader();
	init_texture(L"../../media/textures/Cage.png");

	return true;
}

void BlendApp::v_Render()
{
	//Render scene 

	D3DXCOLOR bgColor( 0.0f, 0.0f, 0.0f, 1.0f );
	m_pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView, bgColor);
	m_pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	TestShader.use(m_pD3D11DeviceContext);

	static float rot = 0.0f;
	rot += .0001f;
	if(rot > 6.26f)
		rot = 0.0f;

	m_pD3D11DeviceContext->PSSetShaderResources( 0, 1, &m_pTexture );
	m_pD3D11DeviceContext->PSSetSamplers( 0, 1, &m_pTexSamplerState );
	m_pD3D11DeviceContext->RSSetState(m_pRasterState);

	//////////////////////Define cube1's world space matrix//////////////////////
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Model  = XMMatrixTranslation( 0.0f, 0.0f, 4.0f );
	Model *= XMMatrixRotationAxis( rotaxis, rot);

	MVP = (Model * View * Proj);
	MVP = XMMatrixTranspose(MVP);
	XMStoreFloat4x4(&cbMatrix.MVP, MVP);
	m_pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	m_pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
    m_pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);


	//////////////////////Define cube2's world space matrix/////////////////////////////
	Model  = XMMatrixRotationAxis( rotaxis, -rot);
	Model *= XMMatrixScaling( 1.3f, 1.3f, 1.3f );

	MVP = (Model * View * Proj);
	MVP = XMMatrixTranspose(MVP);
	XMStoreFloat4x4(&cbMatrix.MVP, MVP);
	m_pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	m_pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);
	m_pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

	m_pSwapChain->Present(0, 0);
}

bool BlendApp::init_device()
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

	hr = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	m_pD3D11DeviceContext->RSSetState(m_pRasterState);


	/////////////////Blend state////////////////////////////////////////
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

	return true;
}

bool BlendApp::init_buffer()
{
	HRESULT result;

	///////////////////////////Index Buffer ////////////////////////////////
	m_VertexCount = 24;
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
	result = m_pD3D11Device->CreateBuffer(&VertexBufferDesc, &VBO, &m_pVertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

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

	result = m_pD3D11Device->CreateBuffer(&IndexBufferDesc, &IBO, &m_pIndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////

	// Set vertex buffer stride and offset.=
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex); 
	offset = 0;
	m_pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	////////////////////////////////Const Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(XMMATRIX);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	m_pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);

	return true;
}

bool BlendApp::init_camera()
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

bool BlendApp::init_shader()
{
	HRESULT result;

	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[2];
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
	pInputLayoutDesc[1].AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;
	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	TestShader.init(m_pD3D11Device, GetHwnd());
	TestShader.attachVS(L"triangle.vsh", pInputLayoutDesc, numElements);
	TestShader.attachPS(L"triangle.psh");
	TestShader.end();

	return true;
}

void BlendApp::init_texture(LPCWSTR texFile)
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