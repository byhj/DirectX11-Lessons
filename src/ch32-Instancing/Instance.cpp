#include "Instance.h"
#include "DirectXTK/WICTextureLoader.h"

void Instance::Render(ID3D11DeviceContext *pD3D11DeviceContext, XMFLOAT4X4 Model, XMFLOAT4X4  View, XMFLOAT4X4 Proj)
{

	unsigned int stride;
	unsigned int offset;

	cbMatrix.model  = Model;
	cbMatrix.view   = View;
	cbMatrix.proj   = Proj;

	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer.Get(), 0, NULL, &cbMatrix, 0, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, m_pMVPBuffer.GetAddressOf());

	pD3D11DeviceContext->VSSetConstantBuffers(2, 1, m_pLeaveMatrixBuffer.GetAddressOf());
	pD3D11DeviceContext->VSSetConstantBuffers(3, 1, m_pTreeMatrixBuffer.GetAddressOf());
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pD3D11DeviceContext->PSSetSamplers(0, 1, m_pTexSamplerState.GetAddressOf());

	InstanceShader.use(pD3D11DeviceContext);

	///////////////////////////////////////////////////////////////////////////////////
	stride = sizeof(MeshStruct::Vertex);
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pLeaveVB, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pLeaveIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->PSSetShaderResources(0, 1, m_pLeaveTexSRV.GetAddressOf());

	cbInstance.isTree = 0.0f;
	cbInstance.isLeaf = 2.0f;
	pD3D11DeviceContext->UpdateSubresource(m_pInstanceBuffer.Get(), 0, NULL, &cbInstance, 0, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(1, 1, m_pInstanceBuffer.GetAddressOf());
	pD3D11DeviceContext->DrawIndexedInstanced(6, NumTrees * NumLeaves, 0, 0, 0);

	///////////////////////////////////////////////////////////////////////////////////
	stride = sizeof(MeshStruct::Vertex);
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pTreeVB, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pTreeIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->PSSetShaderResources(0, 1, m_pTreeTexSRV.GetAddressOf());

	cbInstance.isTree = 2.0f;
	cbInstance.isLeaf = 0.0f;
	pD3D11DeviceContext->UpdateSubresource(m_pInstanceBuffer.Get(), 0, NULL, &cbInstance, 0, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(1, 1, m_pInstanceBuffer.GetAddressOf());
	pD3D11DeviceContext->DrawIndexedInstanced(treeModel.GetIndexCount(), NumTrees, 0, 0, 0);
}

bool Instance::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	treeModel.initModel(pD3D11Device, pD3D11DeviceContext, hWnd);
	treeModel.loadModel("../../media/objects/tree.obj");

	HRESULT hr;

	/************************************New Stuff****************************************************/
	// Create Leaf geometry (quad)
	MeshStruct::Vertex LeaveVertexData[] =
	{
		// Front Face
		XMFLOAT3(-1.0f, -1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(-1.0f,  1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3( 1.0f,  1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3( 1.0f, -1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
	};

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC LeaveVBDesc;
	LeaveVBDesc.Usage               = D3D11_USAGE_DEFAULT;
	LeaveVBDesc.ByteWidth           = sizeof(MeshStruct::Vertex) * 4;
	LeaveVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	LeaveVBDesc.CPUAccessFlags      = 0;
	LeaveVBDesc.MiscFlags           = 0;
	LeaveVBDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA LeaveVBO;
	LeaveVBO.pSysMem          = &LeaveVertexData[0];
	LeaveVBO.SysMemPitch      = 0;
	LeaveVBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = pD3D11Device->CreateBuffer(&LeaveVBDesc, &LeaveVBO, &m_pLeaveVB);
	//DebugHR(hr);


	DWORD LeaveIndexData[] = 
	{
		// Front Face
		0,  1,  2,
		2,  3,  0,
	};

	D3D11_BUFFER_DESC LeaveIBDesc;
	LeaveIBDesc.Usage               = D3D11_USAGE_DEFAULT;
	LeaveIBDesc.ByteWidth           = sizeof(DWORD) * 6;
	LeaveIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	LeaveIBDesc.CPUAccessFlags      = 0;
	LeaveIBDesc.MiscFlags           = 0;
	LeaveIBDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	 D3D11_SUBRESOURCE_DATA LeaveIBO;
	 LeaveIBO.pSysMem          = & LeaveIndexData[0];
	 LeaveIBO.SysMemPitch      = 0;
	 LeaveIBO.SysMemSlicePitch = 0;

	hr = pD3D11Device->CreateBuffer(&LeaveIBDesc, &LeaveIBO, &m_pLeaveIB);
	//DebugHR(hr);


	///////////////////////////Index Buffer ////////////////////////////////


	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC TreeVBDesc;
	TreeVBDesc.Usage               = D3D11_USAGE_DEFAULT;
	TreeVBDesc.ByteWidth           = sizeof(MeshStruct::Vertex) * treeModel.GetVertexCount();
	TreeVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	TreeVBDesc.CPUAccessFlags      = 0;
	TreeVBDesc.MiscFlags           = 0;
	TreeVBDesc.StructureByteStride = 0;
	std::vector<XMFLOAT3> TreeVertex = treeModel.GetVertexData();

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA TreeVBO;
	TreeVBO.pSysMem          = &TreeVertex[0];
	TreeVBO.SysMemPitch      = 0;
	TreeVBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = pD3D11Device->CreateBuffer(&TreeVBDesc, &TreeVBO, &m_pTreeVB);
	//DebugHR(hr);


	D3D11_BUFFER_DESC TreeIBDesc;
	TreeIBDesc.Usage               = D3D11_USAGE_DEFAULT;
	TreeIBDesc.ByteWidth           = sizeof(DWORD) * treeModel.GetIndexCount();
	TreeIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	TreeIBDesc.CPUAccessFlags      = 0;
	TreeIBDesc.MiscFlags           = 0;
	TreeIBDesc.StructureByteStride = 0;

	std::vector<DWORD> TreeIndex = treeModel.GetIndexData();
	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA TreeIBO;
	TreeIBO.pSysMem          = &TreeIndex[0];
	TreeIBO.SysMemPitch      = 0;
	TreeIBO.SysMemSlicePitch = 0;

	hr = pD3D11Device->CreateBuffer(& TreeIBDesc, &TreeIBO, &m_pTreeIB);
	//DebugHR(hr);


	////////////////////////////////MVP Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpBufferDesc;	
	ZeroMemory(&mvpBufferDesc, sizeof(D3D11_BUFFER_DESC));
	mvpBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpBufferDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpBufferDesc.CPUAccessFlags = 0;
	mvpBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpBufferDesc, NULL, &m_pMVPBuffer);
	//DebugHR(hr);

	XMVECTOR tempPos;
	srand(100);
	for(int i = 0; i < NumTrees; i++)
	{
		float randX = ((float)(rand() % 2000) / 10) - 100;
		float randZ = ((float)(rand() % 2000) / 10) - 100;
		tempPos = XMVectorSet(randX, 0.0f, randZ, 0.0f);

		XMStoreFloat4(&cbTreeMatrix.TreeModel[i], tempPos);
	}
	D3D11_BUFFER_DESC treeMatrixBufferDesc;	
	ZeroMemory(&treeMatrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	treeMatrixBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	treeMatrixBufferDesc.ByteWidth      = sizeof(TreeMatrixBuffer);
	treeMatrixBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	treeMatrixBufferDesc.CPUAccessFlags = 0;
	treeMatrixBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA treeMatrixVBO;
	treeMatrixVBO.pSysMem          = &cbTreeMatrix.TreeModel[0];
	treeMatrixVBO.SysMemPitch      = 0;
	treeMatrixVBO.SysMemSlicePitch = 0;

	hr = pD3D11Device->CreateBuffer(&treeMatrixBufferDesc, &treeMatrixVBO, &m_pTreeMatrixBuffer);
	//DebugHR(hr);

	srand(100);
	XMFLOAT3 fTPos;
	XMMATRIX rotationMatrix;
	XMMATRIX tempMatrix;
	for(int i = 0; i < NumLeaves; i++)
	{
		float rotX =(rand() % 2000) / 500.0f; // Value between 0 and 4 PI (two circles, makes it slightly more mixed)
		float rotY = (rand() % 2000) / 500.0f;
		float rotZ = (rand() % 2000) / 500.0f;
		float distFromCenter = 6.0f - ((rand() % 1000) / 250.0f);	

		if(distFromCenter > 4.0f)
			distFromCenter = 4.0f;

		tempPos = XMVectorSet(distFromCenter, 0.0f, 0.0f, 0.0f);
		rotationMatrix = XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix );

		if(XMVectorGetY(tempPos) < -1.0f)
			tempPos = XMVectorSetY(tempPos, -XMVectorGetY(tempPos));

		XMStoreFloat3(&fTPos, tempPos);

		XMMATRIX Scale = XMMatrixScaling( 0.25f, 0.25f, 0.25f );
		XMMATRIX Translation = XMMatrixTranslation(fTPos.x, fTPos.y + 8.0f, fTPos.z );
		tempMatrix = Scale * rotationMatrix * Translation;

		// To make things simple, we just store the matrix directly into our cbPerInst structure
		cbLeaveMatrix.LeaveModel[i] = XMMatrixTranspose(tempMatrix);
	}

	D3D11_BUFFER_DESC leaveMatrixBufferDesc;	
	ZeroMemory(&leaveMatrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	leaveMatrixBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	leaveMatrixBufferDesc.ByteWidth      = sizeof(LeaveMatrixBuffer);
	leaveMatrixBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	leaveMatrixBufferDesc.CPUAccessFlags = 0;
	leaveMatrixBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA leaveMatrixVBO;
	leaveMatrixVBO.pSysMem          = &cbLeaveMatrix.LeaveModel[0];
	leaveMatrixVBO.SysMemPitch      = 0;
	leaveMatrixVBO.SysMemSlicePitch = 0;

	hr = pD3D11Device->CreateBuffer(&leaveMatrixBufferDesc, &leaveMatrixVBO, &m_pLeaveMatrixBuffer);
	//DebugHR(hr);


	D3D11_BUFFER_DESC instanceBufferDesc;	
	ZeroMemory(&instanceBufferDesc, sizeof(D3D11_BUFFER_DESC));
	instanceBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth      = sizeof(InstanceBuffer);
	instanceBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&instanceBufferDesc, NULL, &m_pInstanceBuffer);
	//DebugHR(hr);

	///////////////////////////////////////Light buffer////////////////////////////////////////
	D3D11_BUFFER_DESC lightBufferDesc;	
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth      = sizeof(LightBuffer);
	lightBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags      = 0;

	hr = pD3D11Device->CreateBuffer(&lightBufferDesc, NULL, &m_pLightBuffer);
	//DebugHR(hr);

	hr = CreateWICTextureFromFile(pD3D11Device, L"../../media/objects/treeBark.jpg", NULL, &m_pTreeTexSRV);
	//DebugHR(hr);																						 
	hr = CreateWICTextureFromFile(pD3D11Device, L"../../media/objects/leaf.png",  NULL, &m_pLeaveTexSRV);

    init_texture(pD3D11Device);

	return true;
}


bool Instance::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	HRESULT result;


	std::vector<D3D11_INPUT_ELEMENT_DESC> vInputLayoutDesc;
	D3D11_INPUT_ELEMENT_DESC inputLayout;

	inputLayout.SemanticName         = "POSITION";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 0;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "TEXCOORD";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 12;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "NORMAL";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 20;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "TANGENT";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 32;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "BITANGENT";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 44;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(inputLayout);

	InstanceShader.init(pD3D11Device, vInputLayoutDesc);
	InstanceShader.attachVS(L"instance.vsh", "VS", "vs_5_0");
	InstanceShader.attachPS(L"instance.psh", "PS", "ps_5_0");
	InstanceShader.end();

	return true;
}

void Instance::init_texture(ID3D11Device *pD3D11Device)
{

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

	HRESULT hr;
	hr = pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	//DebugHR(hr);

}
