#include "md5Model.h"

void MD5Model::Render(ID3D11DeviceContext *pD3D11DeviceContext,  XMFLOAT4X4 Model, XMFLOAT4X4 View, XMFLOAT4X4 Proj)
{

	cbMatrix.model  = Model;
	cbMatrix.view   = View;
	cbMatrix.proj   = Proj;
	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, m_pMVPBuffer.GetAddressOf());

	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexType); 
	offset = 0;

	for(int i = 0; i < NewMD5Model.numSubsets; i ++)
	{
		pD3D11DeviceContext->IASetIndexBuffer( NewMD5Model.subsets[i].indexBuff, DXGI_FORMAT_R32_UINT, 0);
		pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &NewMD5Model.subsets[i].vertBuff, &stride, &offset );

		pD3D11DeviceContext->PSSetShaderResources( 0, 1, &m_pTexture[NewMD5Model.subsets[i].texArrayIndex] );
		pD3D11DeviceContext->PSSetSamplers( 0, 1, m_pTexSamplerState.GetAddressOf());
		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		CubeShader.use(pD3D11DeviceContext);
		//pD3D11DeviceContext->RSSetState(RSCullNone);
		pD3D11DeviceContext->DrawIndexed( NewMD5Model.subsets[i].indices.size(), 0, 0 );
	}



}


void MD5Model::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	init_texture(pD3D11Device);
	////////////////////////////////MVP Buffer//////////////////////////////////////
	LoadMD5Model(pD3D11Device, L"../../media/objects/Female/Female.md5mesh",  NewMD5Model, m_pTexture, texFile);

	D3D11_BUFFER_DESC mvpBufferDesc;	
	ZeroMemory(&mvpBufferDesc, sizeof(D3D11_BUFFER_DESC));
	mvpBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpBufferDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpBufferDesc.CPUAccessFlags = 0;
	mvpBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpBufferDesc, NULL, &m_pMVPBuffer);
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

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Lock the light constant buffer so it can be written to.
	hr = pD3D11DeviceContext->Map(m_pLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//DebugHR(hr);

	// Get a pointer to the data in the constant buffer.
	LightBuffer *dataPtr2 = (LightBuffer*)mappedResource.pData;

	dataPtr2->ambientColor   = XMFLOAT4(0.15f, 0.15f, 0.15f, 0.15f);
	dataPtr2->diffuseColor   = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	dataPtr2->lightDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);


	pD3D11DeviceContext->Unmap(m_pLightBuffer.Get(), 0);

	int lightSlot = 0;
	pD3D11DeviceContext->PSSetConstantBuffers(lightSlot, 1, &m_pLightBuffer);

}


bool MD5Model::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	HRESULT result;

	std::vector<D3D11_INPUT_ELEMENT_DESC> vpInputLayoutDesc;
	D3D11_INPUT_ELEMENT_DESC inputLayout;

	inputLayout.SemanticName         = "POSITION";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 0;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vpInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "TEXCOORD";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 12;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vpInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "NORMAL";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 20;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vpInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "TANGENT";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 32;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vpInputLayoutDesc.push_back(inputLayout);

	inputLayout.SemanticName         = "BITANGENT";
	inputLayout.SemanticIndex        = 0;
	inputLayout.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayout.InputSlot            = 0;
	inputLayout.AlignedByteOffset    = 44;
	inputLayout.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputLayout.InstanceDataStepRate = 0;
	vpInputLayoutDesc.push_back(inputLayout);

	CubeShader.init(pD3D11Device, vInputLayoutDesc);
	CubeShader.attachVS(L"md5.vsh", vpInputLayoutDesc);
	CubeShader.attachPS(L"md5.psh");
	CubeShader.end();

	return true;
}

void MD5Model::init_texture(ID3D11Device *pD3D11Device)
{
	HRESULT hr;
	//hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, texFile, NULL,NULL, &m_pTexture, NULL);
	////DebugHR(hr);

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


bool MD5Model::LoadMD5Model(ID3D11Device *pD3D11Device, std::wstring filename, Model3D& MD5Model,
	std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
	std::vector<std::wstring> texFileNameArray)
{
	HRESULT hr;

	std::wifstream fileIn (filename.c_str());		// Open file
	std::wstring checkString;						// Stores the next string from our file
	if(fileIn)										// Check if the file was opened
	{
		while(fileIn)								// Loop until the end of the file is reached
		{	
			fileIn >> checkString;					// Get next string from file

			if(checkString == L"MD5Version")		// Get MD5 version (this function supports version 10)
			{
				/*fileIn >> checkString;
				MessageBox(0, checkString.c_str(),	//display message
				L"MD5Version", MB_OK);*/
			}
			else if ( checkString == L"commandline" )
			{
				std::getline(fileIn, checkString);	// Ignore the rest of this line
			}
			else if ( checkString == L"numJoints" )
			{
				fileIn >> MD5Model.numJoints;		// Store number of joints
			}
			else if ( checkString == L"numMeshes" )
			{
				fileIn >> MD5Model.numSubsets;		// Store number of meshes or subsets which we will call them
			}
			else if ( checkString == L"joints" )
			{
				Joint tempJoint;

				fileIn >> checkString;				// Skip the "{"

				for(int i = 0; i < MD5Model.numJoints; i++)
				{
					fileIn >> tempJoint.name;		// Store joints name
					// Sometimes the names might contain spaces. If that is the case, we need to continue
					// to read the name until we get to the closing " (quotation marks)
					if(tempJoint.name[tempJoint.name.size()-1] != '"')
					{
						wchar_t checkChar;
						bool jointNameFound = false;
						while(!jointNameFound)
						{
							checkChar = fileIn.get();

							if(checkChar == '"')
								jointNameFound = true;		

							tempJoint.name += checkChar;															
						}
					}

					fileIn >> tempJoint.parentID;	// Store Parent joint's ID

					fileIn >> checkString;			// Skip the "("

					// Store position of this joint (swap y and z axis if model was made in RH Coord Sys)
					fileIn >> tempJoint.pos.x >> tempJoint.pos.z >> tempJoint.pos.y;

					fileIn >> checkString >> checkString;	// Skip the ")" and "("

					// Store orientation of this joint
					fileIn >> tempJoint.orientation.x >> tempJoint.orientation.z >> tempJoint.orientation.y;

					// Remove the quotation marks from joints name
					tempJoint.name.erase(0, 1);
					tempJoint.name.erase(tempJoint.name.size()-1, 1);

					// Compute the w axis of the quaternion (The MD5 model uses a 3D vector to describe the
					// direction the bone is facing. However, we need to turn this into a quaternion, and the way
					// quaternions work, is the xyz values describe the axis of rotation, while the w is a value
					// between 0 and 1 which describes the angle of rotation)
					float t = 1.0f - ( tempJoint.orientation.x * tempJoint.orientation.x )
						- ( tempJoint.orientation.y * tempJoint.orientation.y )
						- ( tempJoint.orientation.z * tempJoint.orientation.z );
					if ( t < 0.0f )
					{
						tempJoint.orientation.w = 0.0f;
					}
					else
					{
						tempJoint.orientation.w = -sqrtf(t);
					}

					std::getline(fileIn, checkString);		// Skip rest of this line

					MD5Model.joints.push_back(tempJoint);	// Store the joint into this models joint vector
				}

				fileIn >> checkString;					// Skip the "}"
			}
			else if ( checkString == L"mesh")
			{
				ModelSubset subset;
				int numVerts, numTris, numWeights;

				fileIn >> checkString;					// Skip the "{"

				fileIn >> checkString;
				while ( checkString != L"}" )			// Read until '}'
				{
					// In this lesson, for the sake of simplicity, we will assume a textures filename is givin here.
					// Usually though, the name of a material (stored in a material library. Think back to the lesson on
					// loading .obj files, where the material library was contained in the file .mtl) is givin. Let this
					// be an exercise to load the material from a material library such as obj's .mtl file, instead of
					// just the texture like we will do here.
					if(checkString == L"shader")		// Load the texture or material
					{						
						std::wstring fileNamePath;
						fileIn >> fileNamePath;			// Get texture's filename

						// Take spaces into account if filename or material name has a space in it
						if(fileNamePath[fileNamePath.size()-1] != '"')
						{
							wchar_t checkChar;
							bool fileNameFound = false;
							while(!fileNameFound)
							{
								checkChar = fileIn.get();

								if(checkChar == '"')
									fileNameFound = true;

								fileNamePath += checkChar;																	
							}
						}

						// Remove the quotation marks from texture path
						fileNamePath.erase(0, 1);
						fileNamePath.erase(fileNamePath.size()-1, 1);

						//check if this texture has already been loaded
						bool alreadyLoaded = false;
						std::wstring texDir = L"../../media/objects/Female/";

						for(int i = 0; i < texFileNameArray.size(); ++i)
						{
							if(fileNamePath == texFileNameArray[i])
							{
								alreadyLoaded = true;
								subset.texArrayIndex = i;
							}
						}

						//if the texture is not already loaded, load it now
						if(!alreadyLoaded)
						{
							ID3D11ShaderResourceView* tempMeshSRV;
							hr = D3DX11CreateShaderResourceViewFromFile( pD3D11Device, (texDir + fileNamePath).c_str(),
								NULL, NULL, &tempMeshSRV, NULL );
							if(SUCCEEDED(hr))
							{
								texFileNameArray.push_back(fileNamePath.c_str());
								subset.texArrayIndex = shaderResourceViewArray.size();
								shaderResourceViewArray.push_back(tempMeshSRV);
							}
							else
							{
								MessageBox(0, fileNamePath.c_str(),		//display message
									L"Could Not Open:", MB_OK);
								return false;
							}
						}	

						std::getline(fileIn, checkString);				// Skip rest of this line
					}
					else if ( checkString == L"numverts")
					{
						fileIn >> numVerts;								// Store number of vertices

						std::getline(fileIn, checkString);				// Skip rest of this line

						for(int i = 0; i < numVerts; i++)
						{
							VertexType tempVert;

							fileIn >> checkString						// Skip "vert # ("
								   >> checkString
								   >> checkString;

							fileIn >> tempVert.texCoord.x				// Store tex coords
								   >> tempVert.texCoord.y;	
							 
							fileIn >> checkString;						// Skip ")"

							fileIn >> tempVert.StartWeight;				// Index of first weight this vert will be weighted to

							fileIn >> tempVert.WeightCount;				// Number of weights for this VertexType

							std::getline(fileIn, checkString);			// Skip rest of this line

							subset.vertices.push_back(tempVert);		// Push back this VertexType into subsets VertexType vector
						}
					}
					else if ( checkString == L"numtris")
					{
						fileIn >> numTris;
						subset.numTriangles = numTris;

						std::getline(fileIn, checkString);				// Skip rest of this line

						for(int i = 0; i < numTris; i++)				// Loop through each triangle
						{
							DWORD tempIndex;
							fileIn >> checkString;						// Skip "tri"
							fileIn >> checkString;						// Skip tri counter

							for(int k = 0; k < 3; k++)					// Store the 3 indices
							{
								fileIn >> tempIndex;
								subset.indices.push_back(tempIndex);
							}

							std::getline(fileIn, checkString);			// Skip rest of this line
						}
					}
					else if ( checkString == L"numweights")
					{
						fileIn >> numWeights;

						std::getline(fileIn, checkString);				// Skip rest of this line

						for(int i = 0; i < numWeights; i++)
						{
							Weight tempWeight;
							fileIn >> checkString >> checkString;		// Skip "weight #"

							fileIn >> tempWeight.jointID;				// Store weight's joint ID

							fileIn >> tempWeight.bias;					// Store weight's influence over a VertexType

							fileIn >> checkString;						// Skip "("

							fileIn >> tempWeight.pos.x					// Store weight's pos in joint's local space
								   >> tempWeight.pos.z
								   >> tempWeight.pos.y;

							std::getline(fileIn, checkString);			// Skip rest of this line

							subset.weights.push_back(tempWeight);		// Push back tempWeight into subsets Weight array
						}

					}
					else
						std::getline(fileIn, checkString);				// Skip anything else

					fileIn >> checkString;								// Skip "}"
				}

				//*** find each VertexType's position using the joints and weights ***//
				for ( int i = 0; i < subset.vertices.size(); ++i )
				{
					VertexType tempVert = subset.vertices[i];
					tempVert.pos = XMFLOAT3(0, 0, 0);	

					// Sum up the joints and weights information to get VertexType's position
					for ( int j = 0; j < tempVert.WeightCount; ++j )
					{
						Weight tempWeight = subset.weights[tempVert.StartWeight + j];
						Joint tempJoint = MD5Model.joints[tempWeight.jointID];

						XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
						XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);
						XMVECTOR tempJointOrientationConjugate = XMVectorSet(-tempJoint.orientation.x, -tempJoint.orientation.y, -tempJoint.orientation.z, tempJoint.orientation.w);

		
						XMFLOAT3 rotatedPoint;
						XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

						tempVert.pos.x += ( tempJoint.pos.x + rotatedPoint.x ) * tempWeight.bias;
						tempVert.pos.y += ( tempJoint.pos.y + rotatedPoint.y ) * tempWeight.bias;
						tempVert.pos.z += ( tempJoint.pos.z + rotatedPoint.z ) * tempWeight.bias;
					}

					subset.positions.push_back(tempVert.pos);			
				}

				// Put the positions into the vertices for this subset
				for(int i = 0; i < subset.vertices.size(); i++)
				{
					subset.vertices[i].pos = subset.positions[i];
				}

				//*** Calculate VertexType normals using normal averaging ***///
				std::vector<XMFLOAT3> tempNormal;

				//normalized and unnormalized normals
				XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

				//Used to get vectors (sides) from the position of the verts
				float vecX, vecY, vecZ;

				//Two edges of our triangle
				XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

				//Compute face normals
				for(int i = 0; i < subset.numTriangles; ++i)
				{
					//Get the vector describing one edge of our triangle (edge 0,2)
					vecX = subset.vertices[subset.indices[(i*3)]].pos.x - subset.vertices[subset.indices[(i*3)+2]].pos.x;
					vecY = subset.vertices[subset.indices[(i*3)]].pos.y - subset.vertices[subset.indices[(i*3)+2]].pos.y;
					vecZ = subset.vertices[subset.indices[(i*3)]].pos.z - subset.vertices[subset.indices[(i*3)+2]].pos.z;		
					edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

					//Get the vector describing another edge of our triangle (edge 2,1)
					vecX = subset.vertices[subset.indices[(i*3)+2]].pos.x - subset.vertices[subset.indices[(i*3)+1]].pos.x;
					vecY = subset.vertices[subset.indices[(i*3)+2]].pos.y - subset.vertices[subset.indices[(i*3)+1]].pos.y;
					vecZ = subset.vertices[subset.indices[(i*3)+2]].pos.z - subset.vertices[subset.indices[(i*3)+1]].pos.z;		
					edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

					//Cross multiply the two edge vectors to get the un-normalized face normal
					XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

					tempNormal.push_back(unnormalized);
				}

				//Compute VertexType normals (normal Averaging)
				XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				int facesUsing = 0;
				float tX, tY, tZ;	//temp axis variables

				//Go through each VertexType
				for(int i = 0; i < subset.vertices.size(); ++i)
				{
					//Check which triangles use this VertexType
					for(int j = 0; j < subset.numTriangles; ++j)
					{
						if(subset.indices[j*3] == i ||
							subset.indices[(j*3)+1] == i ||
							subset.indices[(j*3)+2] == i)
						{
							tX = XMVectorGetX(normalSum) + tempNormal[j].x;
							tY = XMVectorGetY(normalSum) + tempNormal[j].y;
							tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

							normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the VertexType, add the unormalized face normal to the normalSum

							facesUsing++;
						}
					}

					//Get the actual normal by dividing the normalSum by the number of faces sharing the VertexType
					normalSum = normalSum / facesUsing;

					//Normalize the normalSum vector
					normalSum = XMVector3Normalize(normalSum);

					//Store the normal and tangent in our current VertexType
					subset.vertices[i].normal.x = -XMVectorGetX(normalSum);
					subset.vertices[i].normal.y = -XMVectorGetY(normalSum);
					subset.vertices[i].normal.z = -XMVectorGetZ(normalSum);
					
					//Clear normalSum, facesUsing for next VertexType
					normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
					facesUsing = 0;
				}

				// Create index buffer
				D3D11_BUFFER_DESC indexBufferDesc;
				ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

				indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				indexBufferDesc.ByteWidth = sizeof(DWORD) * subset.numTriangles * 3;
				indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				indexBufferDesc.CPUAccessFlags = 0;
				indexBufferDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA iinitData;

				iinitData.pSysMem = &subset.indices[0];
				pD3D11Device->CreateBuffer(&indexBufferDesc, &iinitData, &subset.indexBuff);

				//Create VertexType Buffer
				D3D11_BUFFER_DESC vertexBufferDesc;
				ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

				vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;							// We will be updating this buffer, so we must set as dynamic
				vertexBufferDesc.ByteWidth = sizeof( VertexType ) * subset.vertices.size();
				vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// Give CPU power to write to buffer
				vertexBufferDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA vertexBufferData; 

				ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
				vertexBufferData.pSysMem = &subset.vertices[0];
				hr = pD3D11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &subset.vertBuff);

				// Push back the temp subset into the models subset vector
				MD5Model.subsets.push_back(subset);
			}
		}
	}
	else
	{
		//pSwapChain->SetFullscreenState(false, NULL);	// Make sure we are out of fullscreen

		// create message
		std::wstring message = L"Could not open: ";
		message += filename;

		MessageBox(0, message.c_str(),	// display message
			L"Error", MB_OK);

		return false;
	}

	return true;
}