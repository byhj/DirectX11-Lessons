#include "d3dAABB.h"



bool D3DAABB::CullAABB(XMFLOAT4 worldPos)
{
	// This is where we will check all objects for culling. In this lesson, we are only culling the trees, so if the tree is culled,
	// we will not draw it OR it's leaves. You can add other objects in your scene, and check them for culling here

	bool cullFlag = false;

		// Loop through each frustum plane
	for(int planeID = 0; planeID < 6; ++planeID)
	{
		XMVECTOR planeNormal = XMVectorSet(m_FrustumPlanes[planeID].x, m_FrustumPlanes[planeID].y, m_FrustumPlanes[planeID].z, 0.0f);
		float planeConstant = m_FrustumPlanes[planeID].w;

		// Check each axis (x,y,z) to get the AABB vertex furthest away from the direction the plane is facing (plane normal)
		XMFLOAT3 axisVert;

		// x-axis
		if(m_FrustumPlanes[planeID].x < 0.0f)	// Which AABB vertex is furthest down (plane normals direction) the x axis
			axisVert.x = m_AABB[0].x + worldPos.x; // min x plus tree positions x
		else
			axisVert.x = m_AABB[1].x + worldPos.x; // max x plus tree positions x

		// y-axis
		if(m_FrustumPlanes[planeID].y < 0.0f)	// Which AABB vertex is furthest down (plane normals direction) the y axis
			axisVert.y = m_AABB[0].y + worldPos.y; // min y plus tree positions y
		else
			axisVert.y = m_AABB[1].y + worldPos.y; // max y plus tree positions y

		// z-axis
		if(m_FrustumPlanes[planeID].z < 0.0f)	// Which AABB vertex is furthest down (plane normals direction) the z axis
			axisVert.z = m_AABB[0].z + worldPos.z; // min z plus tree positions z
		else
			axisVert.z = m_AABB[1].z + worldPos.z; // max z plus tree positions z

		// Now we get the signed distance from the AABB vertex that's furthest down the frustum planes normal,
		// and if the signed distance is negative, then the entire bounding box is behind the frustum plane, which means
		// that it should be culled
		if(XMVectorGetX(XMVector3Dot(planeNormal, XMLoadFloat3(&axisVert))) + planeConstant < 0.0f)
		{
			cullFlag = true;
			// Skip remaining planes to check and move on to next tree
			break;
		}
	}

    return cullFlag;
}

void D3DAABB::CreateAABB(std::vector<XMFLOAT3> &vertPosArray)
{
	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(UINT i = 0; i < vertPosArray.size(); i++)
	{		
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
		// that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
		// covered by the bounding volume

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, vertPosArray[i].x);	// Find smallest x value in model
		minVertex.y = min(minVertex.y, vertPosArray[i].y);	// Find smallest y value in model
		minVertex.z = min(minVertex.z, vertPosArray[i].z);	// Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vertPosArray[i].x);	// Find largest x value in model
		maxVertex.y = max(maxVertex.y, vertPosArray[i].y);	// Find largest y value in model
		maxVertex.z = max(maxVertex.z, vertPosArray[i].z);	// Find largest z value in model
	}	


	// Our AABB [0] is the min vertex and [1] is the max
	m_AABB[0] = minVertex;
	m_AABB[1] = maxVertex;
}

void D3DAABB::CreateFrustumPlanes(XMMATRIX& viewProj)
{
	// x, y, z, and w represent A, B, C and D in the plane equation
	// where ABC are the xyz of the planes normal, and D is the plane constant

	// Left Frustum Plane
	// Add first column of the matrix to the fourth column
	m_FrustumPlanes[0].x = viewProj._14 + viewProj._11; 
	m_FrustumPlanes[0].y = viewProj._24 + viewProj._21;
	m_FrustumPlanes[0].z = viewProj._34 + viewProj._31;
	m_FrustumPlanes[0].w = viewProj._44 + viewProj._41;

	// Right Frustum Plane
	// Subtract first column of matrix from the fourth column
	m_FrustumPlanes[1].x = viewProj._14 - viewProj._11; 
	m_FrustumPlanes[1].y = viewProj._24 - viewProj._21;
	m_FrustumPlanes[1].z = viewProj._34 - viewProj._31;
	m_FrustumPlanes[1].w = viewProj._44 - viewProj._41;

	// Top Frustum Plane
	// Subtract second column of matrix from the fourth column
	m_FrustumPlanes[2].x = viewProj._14 - viewProj._12; 
	m_FrustumPlanes[2].y = viewProj._24 - viewProj._22;
	m_FrustumPlanes[2].z = viewProj._34 - viewProj._32;
	m_FrustumPlanes[2].w = viewProj._44 - viewProj._42;

	// Bottom Frustum Plane
	// Add second column of the matrix to the fourth column
	m_FrustumPlanes[3].x = viewProj._14 + viewProj._12;
	m_FrustumPlanes[3].y = viewProj._24 + viewProj._22;
	m_FrustumPlanes[3].z = viewProj._34 + viewProj._32;
	m_FrustumPlanes[3].w = viewProj._44 + viewProj._42;

	// Near Frustum Plane
	// We could add the third column to the fourth column to get the near plane,
	// but we don't have to do this because the third column IS the near plane
	m_FrustumPlanes[4].x = viewProj._13;
	m_FrustumPlanes[4].y = viewProj._23;
	m_FrustumPlanes[4].z = viewProj._33;
	m_FrustumPlanes[4].w = viewProj._43;

	// Far Frustum Plane
	// Subtract third column of matrix from the fourth column
	m_FrustumPlanes[5].x = viewProj._14 - viewProj._13; 
	m_FrustumPlanes[5].y = viewProj._24 - viewProj._23;
	m_FrustumPlanes[5].z = viewProj._34 - viewProj._33;
	m_FrustumPlanes[5].w = viewProj._44 - viewProj._43;

	// Normalize plane normals (A, B and C (xyz))
	// Also take note that planes face inward
	for(int i = 0; i < 6; ++i)
	{
		float length = sqrt(  (m_FrustumPlanes[i].x * m_FrustumPlanes[i].x) 
			+ (m_FrustumPlanes[i].y * m_FrustumPlanes[i].y) 
			+ (m_FrustumPlanes[i].z * m_FrustumPlanes[i].z)
			);

		m_FrustumPlanes[i].x /= length;
		m_FrustumPlanes[i].y /= length;
		m_FrustumPlanes[i].z /= length;
		m_FrustumPlanes[i].w /= length;
	}

}
