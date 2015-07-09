#include "d3dBoundSphere.h"

void D3DBoundSphere::CreateBuondSphere(const std::vector<XMFLOAT3> &vPosData)
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


	// Compute distance between maxVertex and minVertex
	float distX = (maxVertex.x - minVertex.x) / 2.0f;
	float distY = (maxVertex.y - minVertex.y) / 2.0f;
	float distZ = (maxVertex.z - minVertex.z) / 2.0f;	

	// Now store the distance between (0, 0, 0) in model space to the models real center
	objectCenterOffset = XMVectorSet(maxVertex.x - distX, maxVertex.y - distY, maxVertex.z - distZ, 0.0f);

	// Compute bounding sphere (distance between min and max bounding box vertices)
	// boundingSphere = sqrt(distX*distX + distY*distY + distZ*distZ) / 2.0f;
	boundingSphere = XMVectorGetX(XMVector3Length(XMVectorSet(distX, distY, distZ, 0.0f)));	
}
