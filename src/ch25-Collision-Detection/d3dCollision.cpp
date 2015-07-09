#include "d3dCollision.h"

bool BoundingSphereCollision(float firstObjBoundingSphere, 
							 XMVECTOR firstObjCenterOffset,
							 XMMATRIX& firstObjWorldSpace,
							 float secondObjBoundingSphere,
							 XMVECTOR secondObjCenterOffset, 
							 XMMATRIX& secondObjWorldSpace)
{
	//Declare local variables
	XMVECTOR world_1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR world_2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	float objectsDistance = 0.0f;

	//Transform the objects world space to objects REAL center in world space
	world_1 = XMVector3TransformCoord(firstObjCenterOffset, firstObjWorldSpace);
	world_2 = XMVector3TransformCoord(secondObjCenterOffset, secondObjWorldSpace);

	//Get the distance between the two objects
	objectsDistance = XMVectorGetX(XMVector3Length(world_1 - world_2));

	//If the distance between the two objects is less than the sum of their bounding spheres...
	if(objectsDistance <= (firstObjBoundingSphere + secondObjBoundingSphere))
		//Return true
			return true;

	//If the bounding spheres are not colliding, return false
	return false;
}


bool BoundingBoxCollision(XMVECTOR& firstObjBoundingBoxMinVertex, 
						  XMVECTOR& firstObjBoundingBoxMaxVertex, 
						  XMVECTOR& secondObjBoundingBoxMinVertex, 
						  XMVECTOR& secondObjBoundingBoxMaxVertex) 
{
	//Is obj1's max X greater than obj2's min X? If not, obj1 is to the LEFT of obj2
	if (XMVectorGetX(firstObjBoundingBoxMaxVertex) > XMVectorGetX(secondObjBoundingBoxMinVertex))

		//Is obj1's min X less than obj2's max X? If not, obj1 is to the RIGHT of obj2
			if (XMVectorGetX(firstObjBoundingBoxMinVertex) < XMVectorGetX(secondObjBoundingBoxMaxVertex))

				//Is obj1's max Y greater than obj2's min Y? If not, obj1 is UNDER obj2
					if (XMVectorGetY(firstObjBoundingBoxMaxVertex) > XMVectorGetY(secondObjBoundingBoxMinVertex))

						//Is obj1's min Y less than obj2's max Y? If not, obj1 is ABOVE obj2
							if (XMVectorGetY(firstObjBoundingBoxMinVertex) < XMVectorGetY(secondObjBoundingBoxMaxVertex)) 

								//Is obj1's max Z greater than obj2's min Z? If not, obj1 is IN FRONT OF obj2
									if (XMVectorGetZ(firstObjBoundingBoxMaxVertex) > XMVectorGetZ(secondObjBoundingBoxMinVertex))

										//Is obj1's min Z less than obj2's max Z? If not, obj1 is BEHIND obj2
											if (XMVectorGetZ(firstObjBoundingBoxMinVertex) < XMVectorGetZ(secondObjBoundingBoxMaxVertex))

												//If we've made it this far, then the two bounding boxes are colliding
													return true;

	//If the two bounding boxes are not colliding, then return false
	return false;
}


void CalculateAABB(std::vector<XMFLOAT3> boundingBoxVerts, 
				   XMMATRIX& worldSpace,
				   XMVECTOR& boundingBoxMin,
				   XMVECTOR& boundingBoxMax)
{
	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	//Loop through the 8 vertices describing the bounding box
	for(UINT i = 0; i < 8; i++)
	{		
		//Transform the bounding boxes vertices to the objects world space
		XMVECTOR Vert = XMVectorSet(boundingBoxVerts[i].x, boundingBoxVerts[i].y, boundingBoxVerts[i].z, 0.0f);
		Vert = XMVector3TransformCoord(Vert, worldSpace);

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, XMVectorGetX(Vert));	// Find smallest x value in model
		minVertex.y = min(minVertex.y, XMVectorGetY(Vert));	// Find smallest y value in model
		minVertex.z = min(minVertex.z, XMVectorGetZ(Vert));	// Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, XMVectorGetX(Vert));	// Find largest x value in model
		maxVertex.y = max(maxVertex.y, XMVectorGetY(Vert));	// Find largest y value in model
		maxVertex.z = max(maxVertex.z, XMVectorGetZ(Vert));	// Find largest z value in model
	}

	//Store Bounding Box's min and max vertices
	boundingBoxMin = XMVectorSet(minVertex.x, minVertex.y, minVertex.z, 0.0f);
	boundingBoxMax = XMVectorSet(maxVertex.x, maxVertex.y, maxVertex.z, 0.0f);
}
