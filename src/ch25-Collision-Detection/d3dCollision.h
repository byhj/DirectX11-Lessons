#ifndef D3DCOLLISION_H
#define D3DCOLLISION_H

#include <windows.h>
#include <DirectXMath.h> using namespace DirectX;
#include <vector>

bool BoundingSphereCollision(float firstObjBoundingSphere, 
							 XMVECTOR firstObjCenterOffset,
							 XMMATRIX& firstObjWorldSpace,
							 float secondObjBoundingSphere,
							 XMVECTOR secondObjCenterOffset, 
							 XMMATRIX& secondObjWorldSpace);


bool BoundingBoxCollision(XMVECTOR& firstObjBoundingBoxMinVertex, 
						  XMVECTOR& firstObjBoundingBoxMaxVertex, 
						  XMVECTOR& secondObjBoundingBoxMinVertex, 
						  XMVECTOR& secondObjBoundingBoxMaxVertex) ;


void CalculateAABB(std::vector<XMFLOAT3> boundingBoxVerts, 
				   XMMATRIX& worldSpace,
				   XMVECTOR& boundingBoxMin,
				   XMVECTOR& boundingBoxMax);


#endif