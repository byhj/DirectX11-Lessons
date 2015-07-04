#ifndef D3DCAMERA_H
#define D3DCAMERA_H

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include <dinput.h>
#include <d3d11.h>
#include <xnamath.h>

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
	void DetectInput(double time , HWND hWnd);
	void UpdateCamera();

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

	float rot;
	float moveLeftRight  ;
	float moveBackForward;

	float camYaw   ;
	float camPitch ;
};


#endif