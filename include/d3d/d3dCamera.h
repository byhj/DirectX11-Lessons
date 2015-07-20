#ifndef D3DCAMERA_H
#define D3DCAMERA_H

#include <windows.h>
#include <d3dx11.h>
#include <dinput.h>
#include <d3d11.h>
#include <xnamath.h>

namespace byhj
{

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
		rightMouseClicked = false;
	}
	bool InitDirectInput(HINSTANCE hInstance , HWND hWnd);
	void DetectInput(double time , HWND hWnd);
	void UpdateCamera();

	XMFLOAT4X4 GetViewMatrix()
	{
		XMStoreFloat4x4(&m_camView, XMMatrixTranspose(camView) );
		return m_camView;
	}
	XMFLOAT4 GetCamPos()
	{
		XMStoreFloat4(&m_camPosition, camPosition);
		return m_camPosition;
	}
	XMFLOAT4  GetCamTarget()
	{
		XMStoreFloat4(&m_camTarget, camTarget);
		return m_camTarget;
	}
	float GetMouseX()
	{
		return m_mouseX;
	}
	float GetMouseY()
	{
		return m_mouseY;
	}
	bool GetRightMouseClicked()
	{
		return rightMouseClicked;
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

	XMFLOAT4   m_camTarget;
	XMFLOAT4   m_camPosition;
	XMFLOAT4X4 m_camView;
	float rot;
	float moveLeftRight  ;
	float moveBackForward;

	float camYaw   ;
	float camPitch ;

	float m_mouseX;
	float m_mouseY;
	bool rightMouseClicked;
};


}

#endif