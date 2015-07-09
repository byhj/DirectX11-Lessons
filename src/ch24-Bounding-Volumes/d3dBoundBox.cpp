#include "d3dBoundBox.h"

void D3DBoundBox::CreateBoundBox(const std::vector<XMFLOAT3> &vPosData)
{
	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(UINT i = 0; i < vPosData.size(); i++)
	{		
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
		// that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
		// covered by the bounding volume

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, vPosData[i].x);	// Find smallest x value in model
		minVertex.y = min(minVertex.y, vPosData[i].y);	// Find smallest y value in model
		minVertex.z = min(minVertex.z, vPosData[i].z);	// Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vPosData[i].x);	// Find largest x value in model
		maxVertex.y = max(maxVertex.y, vPosData[i].y);	// Find largest y value in model
		maxVertex.z = max(maxVertex.z, vPosData[i].z);	// Find largest z value in model
	}

	m_MinVertex = minVertex;
	m_MaxVertex = maxVertex;

	// Create bounding box	
	// Front Vertices
	m_BoxVertexData.push_back(XMFLOAT3(minVertex.x, minVertex.y, minVertex.z));
	m_BoxVertexData.push_back(XMFLOAT3(minVertex.x, maxVertex.y, minVertex.z));
	m_BoxVertexData.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, minVertex.z));
	m_BoxVertexData.push_back(XMFLOAT3(maxVertex.x, minVertex.y, minVertex.z));

	// Back Vertices
	m_BoxVertexData.push_back(XMFLOAT3(minVertex.x, minVertex.y, maxVertex.z));
	m_BoxVertexData.push_back(XMFLOAT3(maxVertex.x, minVertex.y, maxVertex.z));
	m_BoxVertexData.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, maxVertex.z));
	m_BoxVertexData.push_back(XMFLOAT3(minVertex.x, maxVertex.y, maxVertex.z));

	DWORD* i = new DWORD[36];

	// Front Face
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Back Face
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Top Face
	i[12] = 1; i[13] = 7; i[14] = 6;
	i[15] = 1; i[16] = 6; i[17] = 2;

	// Bottom Face
	i[18] = 0; i[19] = 4; i[20] = 5;
	i[21] = 0; i[22] = 5; i[23] = 3;

	// Left Face
	i[24] = 4; i[25] = 7; i[26] = 1;
	i[27] = 4; i[28] = 1; i[29] = 0;

	// Right Face
	i[30] = 3; i[31] = 2; i[32] = 6;
	i[33] = 3; i[34] = 6; i[35] = 5;

	for(int j = 0; j < 36; j++)
		m_BoxIndexData.push_back(i[j]);

}


std::vector<XMFLOAT3> D3DBoundBox::GetPosData()
{
	return m_BoxVertexData;
}

std::vector<DWORD> D3DBoundBox::GetIndexData()
{
	return m_BoxIndexData;
}

XMFLOAT3 D3DBoundBox::GetMinVertex()
{
	return m_MinVertex;
}

XMFLOAT3 D3DBoundBox::GetMaxVertex()
{
	return m_MaxVertex;
}