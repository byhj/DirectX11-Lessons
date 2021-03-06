#include "Cube.h"
#include "d3d/d3dDebug.h"
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>

#include <array>

namespace byhj
{

void Cube::Init(ID3D11Device *pD3D11Device, HWND hWnd)
{
	init_buffer(pD3D11Device);
	init_shader(pD3D11Device, hWnd);
	init_texture(pD3D11Device);
}

void Cube::Render(ID3D11DeviceContext *pD3D11DeviceContext, const byhj::MatrixBuffer &matrix)
{
    m_cbMatrix.Model = matrix.Model;
	m_cbMatrix.View  = matrix.View;
	m_cbMatrix.Proj  = matrix.Proj;
	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer.Get(), 0, NULL, &m_cbMatrix, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, m_pMVPBuffer.GetAddressOf());

	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex); 
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the texture for shader resoucres and the texture  samplers status
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, m_pTexture.GetAddressOf() );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, m_pTexSamplerState.GetAddressOf());

	TestShader.use(pD3D11DeviceContext);

	pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}

void Cube::Shutdown()
{

}

void Cube::init_buffer(ID3D11Device *pD3D11Device)
{
	///////////////////////////Index Buffer ////////////////////////////////
	m_VertexCount = 24;
	std::array<byhj::Vertex, 24> VertexData = 
	{
		// Front Face
		byhj::Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		byhj::Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		byhj::Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		byhj::Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Back Face
		byhj::Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
		byhj::Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		byhj::Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
		byhj::Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

		// Top Face
		byhj::Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
		byhj::Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
		byhj::Vertex( 1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
		byhj::Vertex( 1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

		// Bottom Face
		byhj::Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
		byhj::Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		byhj::Vertex( 1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
		byhj::Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

		// Left Face
		byhj::Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
		byhj::Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
		byhj::Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		byhj::Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Right Face
		byhj::Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		byhj::Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		byhj::Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
		byhj::Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
	};

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC VertexBufferDesc;
	VertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth           = sizeof(byhj::Vertex) * m_VertexCount;
	VertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags      = 0;
	VertexBufferDesc.MiscFlags           = 0;
	VertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA VBO;
	VBO.pSysMem          = &VertexData;
	VBO.SysMemPitch      = 0;
	VBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT hr = pD3D11Device->CreateBuffer(&VertexBufferDesc, &VBO, &m_pVertexBuffer);
    //DebugHR(hr);

	/////////////////////////////////Index Buffer ///////////////////////////////////////
	m_IndexCount = 36;
	std::array<DWORD, 36>  IndexData = 		
	{
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

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC IndexBufferDesc;
	IndexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth           = sizeof(unsigned long) * m_IndexCount;
	IndexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags      = 0;
	IndexBufferDesc.MiscFlags           = 0;
	IndexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem          = &IndexData;
	IBO.SysMemPitch      = 0;
	IBO.SysMemSlicePitch = 0;

	hr = pD3D11Device->CreateBuffer(&IndexBufferDesc, &IBO, &m_pIndexBuffer);

	////////////////////////////////Const Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(byhj::MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
}

void Cube::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	//Shader interface infomation
	D3D11_INPUT_ELEMENT_DESC InputLayout;
	std::vector<D3D11_INPUT_ELEMENT_DESC> vInputLayoutDesc;
	
	InputLayout.SemanticName         = "POSITION";
	InputLayout.SemanticIndex        = 0;
	InputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	InputLayout.InputSlot            = 0;
	InputLayout.AlignedByteOffset    = 0;
	InputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	InputLayout.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(InputLayout);               

	InputLayout.SemanticName         = "TEXCOORD";
	InputLayout.SemanticIndex        = 0;
	InputLayout.Format               = DXGI_FORMAT_R32G32_FLOAT;
	InputLayout.InputSlot            = 0;
	InputLayout.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	InputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	InputLayout.InstanceDataStepRate = 0;

	vInputLayoutDesc.push_back(InputLayout);     

	TestShader.init(pD3D11Device, vInputLayoutDesc);
	TestShader.attachVS(L"cube.vsh", "VS", "vs_5_0");
	TestShader.attachPS(L"cube.psh", "PS", "ps_5_0");
}

void Cube::init_texture(ID3D11Device *pD3D11Device)
{
	LPCWSTR  texFile = L"../../media/textures/byhj.jpg";

	HRESULT hr;
	//Use shaderResourceView to make texture to the shader
	hr = CreateWICTextureFromFile(pD3D11Device, texFile, NULL, &m_pTexture);

	//DebugHR(hr);

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
	hr = pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	//DebugHR(hr);
}
}

