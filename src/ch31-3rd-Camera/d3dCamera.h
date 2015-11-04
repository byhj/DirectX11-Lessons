#ifndef D3DCAMERA_H
#define D3DCAMERA_H


#include <dinput.h>
#include <d3d11.h>
#include <DirectXMath.h> using namespace DirectX;
#include "md5Model.h"


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
		playerCharWorld = XMMatrixIdentity();
		rot = 0.01f;
	}
	bool InitDirectInput(HINSTANCE hInstance , HWND hWnd);
	void DetectInput(double time , HWND hWnd,  MD5Model &md5Model, ID3D11DeviceContext *pDeviceContext);
	void UpdateCamera();
	void MoveChar(double time, XMVECTOR& destinationDirection, XMMATRIX& worldMatrix,  MD5Model &md5Model, ID3D11DeviceContext *pDeviceContext);

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
	XMMATRIX GetCharMatrix()
	{
		return playerCharWorld;
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

	XMMATRIX playerCharWorld;

};


#endif