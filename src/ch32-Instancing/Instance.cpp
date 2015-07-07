#include "Instance.h"


bool Instance::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	/************************************New Stuff****************************************************/
	// Create Leaf geometry (quad)
	Vertex LeaveVertexData[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f,    0.0f, 1.0f,    0.0f, 0.0f, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f,    0.0f, 0.0f,    0.0f, 0.0f, -1.0f),
		Vertex( 1.0f,  1.0f, -1.0f,    1.0f, 0.0f,    0.0f, 0.0f, -1.0f),
		Vertex( 1.0f, -1.0f, -1.0f,    1.0f, 1.0f,    0.0f, 0.0f, -1.0f),
	};

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC LeaveVBDesc;
	LeaveVBDesc.Usage               = D3D11_USAGE_DEFAULT;
	LeaveVBDesc.ByteWidth           = sizeof(Vertex) * 4;
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
	DebugHR(hr);


	DWORD LeaveIndexData[] = 
	{
		// Front Face
		0,  1,  2,
		0,  2,  3,
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
	DebugHR(hr);


	///////////////////////////Index Buffer ////////////////////////////////
	treeModel.loadModel("../../media/objects/tree.obj");

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC TreeVBDesc;
	TreeVBDesc.Usage               = D3D11_USAGE_DEFAULT;
	TreeVBDesc.ByteWidth           = sizeof(MeshStruct::Vertex) * treeModel.GetVertexCount();
	TreeVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	TreeVBDesc.CPUAccessFlags      = 0;
	TreeVBDesc.MiscFlags           = 0;
	TreeVBDesc.StructureByteStride = 0;
	std::vector<MeshStruct::Vertex> TreeVertex = treeModel.GetVertexData();

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA TreeVBO;
	TreeVBO.pSysMem          = &TreeVertex[0];
	TreeVBO.SysMemPitch      = 0;
	TreeVBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = pD3D11Device->CreateBuffer(&TreeVBDesc, &TreeVBO, &m_pTreeVB);
	DebugHR(hr);


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
	DebugHR(hr);


	////////////////////////////////MVP Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpBufferDesc;	
	ZeroMemory(&mvpBufferDesc, sizeof(D3D11_BUFFER_DESC));
	mvpBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpBufferDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpBufferDesc.CPUAccessFlags = 0;
	mvpBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpBufferDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);

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
	DebugHR(hr);

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
	DebugHR(hr);


	D3D11_BUFFER_DESC instanceBufferDesc;	
	ZeroMemory(&instanceBufferDesc, sizeof(D3D11_BUFFER_DESC));
	instanceBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth      = sizeof(InstanceBuffer);
	instanceBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&instanceBufferDesc, NULL, &m_pInstanceBuffer);
	DebugHR(hr);

	///////////////////////////////////////Light buffer////////////////////////////////////////
	D3D11_BUFFER_DESC lightBufferDesc;	
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth      = sizeof(LightBuffer);
	lightBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags      = 0;

	hr = pD3D11Device->CreateBuffer(&lightBufferDesc, NULL, &m_pLightBuffer);
	DebugHR(hr);

	return true;
}


bool Instance::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	HRESULT result;

	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[5];

	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "NORMAL";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	pInputLayoutDesc[2].SemanticName         = "TEXCOORD";
	pInputLayoutDesc[2].SemanticIndex        = 0;
	pInputLayoutDesc[2].Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc[2].InputSlot            = 0;
	pInputLayoutDesc[2].AlignedByteOffset    = 24;
	pInputLayoutDesc[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[2].InstanceDataStepRate = 0;

	pInputLayoutDesc[3].SemanticName         = "TANGENT";
	pInputLayoutDesc[3].SemanticIndex        = 0;
	pInputLayoutDesc[3].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[3].InputSlot            = 0;
	pInputLayoutDesc[3].AlignedByteOffset    = 32;
	pInputLayoutDesc[3].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[3].InstanceDataStepRate = 0;

	pInputLayoutDesc[4].SemanticName         = "BITANGENT";
	pInputLayoutDesc[4].SemanticIndex        = 0;
	pInputLayoutDesc[4].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[4].InputSlot            = 0;
	pInputLayoutDesc[4].AlignedByteOffset    = 44;
	pInputLayoutDesc[4].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[4].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	InstanceShader.init(pD3D11Device, hWnd);
	InstanceShader.attachVS(L"instance.vsh", pInputLayoutDesc, numElements);
	InstanceShader.attachPS(L"instance.psh");
	InstanceShader.end();

	return true;
}

void Instance::init_texture(ID3D11Device *pD3D11Device, LPCWSTR texFile)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, texFile, NULL,NULL, &m_pTexture, NULL);
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
	hr = pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	DebugHR(hr);

}
