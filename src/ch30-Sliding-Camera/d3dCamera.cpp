#include "collision.h"
#include "d3dCamera.h"

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
	hr = m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

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
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);

	/************************************New Stuff****************************************************/
	// First-Person Camera
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);
	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	/*
	// Free-Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);
	*/

	CollisionPacket cameraCP;
	cameraCP.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	cameraCP.w_Position = camPosition;
	cameraCP.w_Velocity = (moveLeftRight*camRight)+(moveBackForward*camForward);

	camPosition = CollisionSlide(cameraCP,
		collidableGeometryPositions,
		collidableGeometryIndices);

	/*camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;*/
	/*************************************************************************************************/

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;	

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

}

