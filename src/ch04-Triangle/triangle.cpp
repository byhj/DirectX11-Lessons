#include "triangle.h"
#include "d3d/d3dDebug.h"

#include <array>

namespace byhj
{

void Triangle::Init(ID3D11Device *pD3D11Device, HWND hWnd)
{
	init_buffer(pD3D11Device);
	init_shader(pD3D11Device, hWnd);
}

void Triangle::Render(ID3D11DeviceContext *pD3D11DeviceContext)
{
	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex); 
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	TestShader.use(pD3D11DeviceContext);

	pD3D11DeviceContext->Draw(3, 0);

}

void Triangle::Shutdown()
{
	ReleaseCOM(m_pVertexBuffer);
}

void Triangle::init_buffer(ID3D11Device *pD3D11Device)
{
	HRESULT result;

	///////////////////////////Index Buffer ////////////////////////////////
	m_VertexCount = 3;
	std::array<Vertex, 3> VertexData;
	VertexData[0].Pos = XMFLOAT3(-0.5f, -0.5f, 0.0f);  // Bottom left.
	VertexData[1].Pos = XMFLOAT3( 0.0f,  0.5f, 0.0f);  // Top middle.
	VertexData[2].Pos = XMFLOAT3( 0.5f, -0.5f, 0.0f);  // Bottom right.

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
	VBO.pSysMem          = &VertexData;
	VBO.SysMemPitch      = 0;
	VBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = pD3D11Device->CreateBuffer(&VertexBufferDesc, &VBO, &m_pVertexBuffer);
   // DebugHR(result);
}

void Triangle::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	//Shader interface infomation
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[1];
	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	TestShader.init(pD3D11Device, hWnd);
	TestShader.attachVS(L"triangle.vsh", pInputLayoutDesc, numElements);
	TestShader.attachPS(L"triangle.psh");
	TestShader.end();
}


}

