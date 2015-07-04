#ifndef D3DCAMERA_H
#define D3DCAMERA_H

#include <dinput.h>
#include <d3d11.h>
#include <xnamath.h>
#include <vector>

class D3DCamera
{
public:
	D3DCamera()
	{
		DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		DefaultRight   = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		camForward     = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		camRight       = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		moveLeftRight   = 0.0f;
		moveBackForward = 0.0f;
		camYaw = 0.0f;
		camPitch = 0.0f;
		camUp  = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		camPosition    = XMVectorSet( 0.0f, 5.0f, -8.0f, 0.0f );
		camTarget      = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
		camView        = XMMatrixLookAtLH(camPosition, camTarget, camUp );
		rot = 0.01f;
	}
	bool InitDirectInput(HINSTANCE hInstance , HWND hWnd);
	void InitPickModel(int sw, int sh, int num, const std::vector<XMFLOAT3> &vPos,
		              const std::vector<unsigned long> &vIndex, XMMATRIX *modelWorld);

	void DetectInput(double time , HWND hWnd);
	void UpdateCamera();

	bool PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point );

	float pick(XMVECTOR pickRayInWorldSpacePos, XMVECTOR pickRayInWorldSpaceDir, 
		       std::vector<XMFLOAT3>& vertPosArray, std::vector<DWORD>& indexPosArray, 
		       XMMATRIX& worldSpace);

	void pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir);

	XMMATRIX GetViewMatrix()
	{
		return camView;
	}
	XMVECTOR GetCamPos()
	{
		return camPosition;
	}
	XMVECTOR GetCamTarget()
	{
		return camTarget;
	}
	int GetScore()
	{
		return score;
	}
	bool IfHit(int index)
	{
		return bottleHit[index];
	}
	
private:
	IDirectInputDevice8* m_pDIKeyboard;
	IDirectInputDevice8* m_pDIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	XMVECTOR DefaultForward ;
	XMVECTOR DefaultRight   ;
	XMVECTOR camForward     ;
	XMVECTOR camRight       ;

	XMMATRIX camRotationMatrix;
	XMMATRIX camView;
	XMMATRIX camProjection;	
	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;

/////////////////////////////////////////////////////////
	float rot;
	float moveLeftRight  ;
	float moveBackForward;

	float camYaw   ;
	float camPitch ;

	int score;
	float pickedDist;
	XMMATRIX *bottleWorld;
	int* bottleHit;
	int numBottles;

	std::vector<XMFLOAT3> vertPosArray;
	std::vector<unsigned long> indexPosArray;
	int ClientWidth; 
	int ClientHeight;
};

void D3DCamera::InitPickModel(int sw, int sh, int num, const std::vector<XMFLOAT3> &vPos,
							  const std::vector<unsigned long> &vIndex, XMMATRIX *modelWorld)
{
	score = 0;
	pickedDist = 0;
	bottleWorld = new XMMATRIX[num];
	bottleHit = new int[num] ;
	for (int i = 0; i != num; ++i)
		bottleHit[i] = 0;

	numBottles = num;
	ClientWidth = sw; 
	ClientHeight = sh;
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)sw/sh, 1.0f, 1000.0f);
	bottleWorld = modelWorld;
	vertPosArray = vPos;
	indexPosArray = vIndex;
}

bool D3DCamera::InitDirectInput(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL); 

	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL);
	hr = DirectInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);

	hr = m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse);
	hr = m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void D3DCamera::DetectInput(double time, HWND hWnd)
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	//Acquire current mouse and key state
	m_pDIKeyboard->Acquire();
	m_pDIMouse->Acquire();
	m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	m_pDIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

	if(keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hWnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * time;
	static bool isShoot = false;
	//Enter ADSW to move camera left right back forword

	if(keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if(keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if(keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if(keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	if(mouseCurrState.rgbButtons[0])
	{
		if(isShoot == false)
		{
			//Get mouse pos
			POINT mousePos;
			GetCursorPos(&mousePos);			
			ScreenToClient(hWnd, &mousePos);
			int mousex = mousePos.x;
			int mousey = mousePos.y;

			float tempDist;
			float closestDist = FLT_MAX;
			int hitIndex;

			//Change the mouse pos to a 3d ray
			XMVECTOR prwsPos, prwsDir;
			pickRayVector(mousex, mousey, prwsPos, prwsDir);

			//Check if picking the object and mark it
			for(int i = 0; i < numBottles; i++)
			{
				if(bottleHit[i] == 0) //No need to check bottles already hit
				{
					tempDist = pick(prwsPos, prwsDir, vertPosArray, indexPosArray, bottleWorld[i] );
					if(tempDist < closestDist)
					{
						closestDist = tempDist;
						hitIndex = i;
					}
				}
			}
			//We pick a object, and set the closest one be the target object
			if(closestDist < FLT_MAX)
			{
				bottleHit[hitIndex] = 1;
				pickedDist = closestDist;
				score++;
			}
			isShoot = true;
		}
	}

	if(!mouseCurrState.rgbButtons[0])
	{
		isShoot = false;
	}

	//Use mouse to change the rotation matrix
	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseCurrState.lX * 0.001f;
		camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}

	UpdateCamera();

	return;
}

void D3DCamera::UpdateCamera()
{	
	//Rotating the Camera by euler angle
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget         = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget         = XMVector3Normalize(camTarget);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	//We only move the xz plane by axis y
	//Update the camera vector
	camRight   = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp      = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	// Free-Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	//Moving the Camera
	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;	

	//Set the camera matrix
	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

}


void D3DCamera::pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir)
{
	XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float PRVecX, PRVecY, PRVecZ;

	//Transform 2D pick position on screen space to 3D ray in View space
	PRVecX =  ((( 2.0f * mouseX) / ClientWidth ) - 1 ) / camProjection(0,0);
	PRVecY = -((( 2.0f * mouseY) / ClientHeight) - 1 ) / camProjection(1,1);
	PRVecZ =  1.0f;	//View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

	pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

	//Uncomment this line if you want to use the center of the screen (client area)
	//to be the point that creates the picking ray (eg. first person shooter)
	//pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Transform 3D Ray from View space to 3D ray in World space
	XMMATRIX pickRayToWorldSpaceMatrix;
	XMVECTOR matInvDeter;	//We don't use this, but the xna matrix inverse function requires the first parameter to not be null

	pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, camView);	//Inverse of View Space matrix is World space matrix

	pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
}

float D3DCamera::pick(XMVECTOR pickRayInWorldSpacePos,
					  XMVECTOR pickRayInWorldSpaceDir, 
					  std::vector<XMFLOAT3>& vertPosArray,
					  std::vector<DWORD>& indexPosArray, 
					  XMMATRIX& worldSpace)
{ 		
	//Loop through each triangle in the object
	for(int i = 0; i < indexPosArray.size()/3; i++)
	{
		//Triangle's vertices V1, V2, V3
		XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//Temporary 3d floats for each vertex
		XMFLOAT3 tV1, tV2, tV3;

		//Get triangle 
		tV1 = vertPosArray[indexPosArray[(i*3)+0]];
		tV2 = vertPosArray[indexPosArray[(i*3)+1]];
		tV3 = vertPosArray[indexPosArray[(i*3)+2]];

		tri1V1 = XMVectorSet(tV1.x, tV1.y, tV1.z, 0.0f);
		tri1V2 = XMVectorSet(tV2.x, tV2.y, tV2.z, 0.0f);
		tri1V3 = XMVectorSet(tV3.x, tV3.y, tV3.z, 0.0f);

		//Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldSpace);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldSpace);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldSpace);

		//Find the normal using U, V coordinates (two edges)
		XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;

		//Compute face normal by crossing U, V
		faceNormal = XMVector3Cross(U, V);

		faceNormal = XMVector3Normalize(faceNormal);

		//Calculate a point on the triangle for the plane equation
		XMVECTOR triPoint = tri1V1;

		//Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1A = XMVectorGetX(faceNormal);
		float tri1B = XMVectorGetY(faceNormal);
		float tri1C = XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A*XMVectorGetX(triPoint) - tri1B*XMVectorGetY(triPoint) - tri1C*XMVectorGetZ(triPoint));

		//Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (XMVectorGetX(pickRayInWorldSpacePos) * tri1A) + (XMVectorGetY(pickRayInWorldSpacePos) * tri1B) + (XMVectorGetZ(pickRayInWorldSpacePos) * tri1C);
		ep2 = (XMVectorGetX(pickRayInWorldSpaceDir) * tri1A) + (XMVectorGetY(pickRayInWorldSpaceDir) * tri1B) + (XMVectorGetZ(pickRayInWorldSpaceDir) * tri1C);

		//Make sure there are no divide-by-zeros
		if(ep2 != 0.0f)
			t = -(ep1 + tri1D)/(ep2);

		if(t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = XMVectorGetX(pickRayInWorldSpacePos) + XMVectorGetX(pickRayInWorldSpaceDir) * t;
			planeIntersectY = XMVectorGetY(pickRayInWorldSpacePos) + XMVectorGetY(pickRayInWorldSpaceDir) * t;
			planeIntersectZ = XMVectorGetZ(pickRayInWorldSpacePos) + XMVectorGetZ(pickRayInWorldSpaceDir) * t;

			pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			//Call function to check if point is in the triangle
			if(PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				//Return the distance to the hit, so you can check all the other pickable objects in your scene
				//and choose whichever object is closest to the camera
				return t/2.0f;
			}
		}
	}
	//return the max float value (near infinity) if an object was not picked
	return FLT_MAX;
}


bool D3DCamera::PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point )
{
	//To find out if the point is inside the triangle, we will check to see if the point
	//is on the correct side of each of the triangles edges.

	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	return false;
}
#endif