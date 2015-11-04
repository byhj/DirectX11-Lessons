#include "triangle.h"
#include "d3d/d3dDebug.h"

#include <array>
#include <vector>

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
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	TestShader.use(pD3D11DeviceContext);

	pD3D11DeviceContext->Draw(3, 0);

}

void Triangle::Shutdown()
{

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

	TestShader.init(pD3D11Device, vInputLayoutDesc);
	TestShader.attachVS(L"triangle.vsh", "VS", "vs_5_0");
	TestShader.attachPS(L"triangle.psh", "PS", "ps_5_0");
}


}

