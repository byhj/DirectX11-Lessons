#include "d3dCamera.h"

#define SHADER_DEBUG

namespace byhj
{

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
	hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse);

#ifdef SHADER_DEBUG
	hr = m_pDIKeyboard->SetCooperativeLevel(hWnd,DISCL_FOREGROUND);
	hr = m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND);
#else
	hr = m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hr = m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
#endif
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
	if(mouseCurrState.rgbButtons[0])
	{
		//Get mouse pos
		POINT mousePos;
		GetCursorPos(&mousePos);			
		ScreenToClient(hWnd, &mousePos);
		m_mouseX = mousePos.x;
		m_mouseY = mousePos.y;
		rightMouseClicked = true;
	}

	if(!mouseCurrState.rgbButtons[0])
	{
		rightMouseClicked = false;
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

}