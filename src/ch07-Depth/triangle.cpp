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
	pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	TestShader.use(pD3D11DeviceContext);

	pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}

void Triangle::Shutdown()
{
	ReleaseCOM(m_pVertexBuffer);
	ReleaseCOM(m_pIndexBuffer);
}

void Triangle::init_buffer(ID3D11Device *pD3D11Device)
{
	///////////////////////////Index Buffer ////////////////////////////////
	m_VertexCount = 4;
	std::array<Vertex, 4> VertexData = 
	{
	    Vertex( -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f ),
		Vertex( -0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f ),
		Vertex(  0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f ),
		Vertex(  0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f ),
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
	VBO.pSysMem          = &VertexData;
	VBO.SysMemPitch      = 0;
	VBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT hr = pD3D11Device->CreateBuffer(&VertexBufferDesc, &VBO, &m_pVertexBuffer);
    //DebugHR(hr);

	/////////////////////////////////Index Buffer ///////////////////////////////////////
	m_IndexCount = 6;
	std::array<DWORD, 6>  IndexData = {0, 1, 2, 0, 2, 3};

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

}

void Triangle::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
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

	InputLayout.SemanticName         = "COLOR";
	InputLayout.SemanticIndex        = 0;
	InputLayout.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	InputLayout.InputSlot            = 0;
	InputLayout.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	InputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	InputLayout.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(InputLayout);    

	TestShader.init(pD3D11Device, vInputLayoutDesc);
	TestShader.attachVS(L"triangle.vsh", "VS", "vs_5_0");
	TestShader.attachPS(L"triangle.psh", "PS", "ps_5_0");
}


}

