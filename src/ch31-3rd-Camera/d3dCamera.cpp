#include "d3dCamera.h"

XMVECTOR currCharDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR oldCharDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
float charCamDist = 15.0f; // This is the distance between the camera and the character

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
void D3DCamera::MoveChar(double time, XMVECTOR& destinationDirection, XMMATRIX& worldMatrix, MD5Model &md5Model, ID3D11DeviceContext *pDeviceContext)
{
	// Normalize our destinated direction vector
	destinationDirection = XMVector3Normalize(destinationDirection);

	// If character is currently facing the complete opposite direction as the desired direction
	// they will turn around VERY slowly, so we want to make sure they turn around at a normal speed
	// by making the old character direction not the exact opposite direction as the current character
	// position. Try commenting out the next two lines to see what i'm talking about
	if(XMVectorGetX(XMVector3Dot(destinationDirection, oldCharDirection)) == -1)
		oldCharDirection += XMVectorSet(0.02f, 0.0f, -0.02f, 0.0f);

	// Get our current characters position in the world, from it's world matrix
	charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	charPosition = XMVector3TransformCoord(charPosition, worldMatrix);

	// Rotate our character smoothly when changing direction (from the GPG series)
	float destDirLength = 10.0f * time;		// Change to the speed you want your character to rotate. This uses the game timer from an earlier lesson
	// The larget this value, the faster the character rotates
	currCharDirection = oldCharDirection + (destinationDirection * destDirLength);	// Get the characters direction (based off time, old position, and desired
	// direction), by adding together the current direction and the old direction
	// to get vector that smoothly turns from oldCharDir to denstinationDirection
	currCharDirection = XMVector3Normalize(currCharDirection);		// Normalize the characters current direction vector

	// Here we find the angle of our character (angle between current direction and world's normal vector), used so that we can actually rotate
	// our characters world matrix. The three lines below, together, find the angle between 0 PI and 2 PI (360 degrees, and technically, it returns
	// the degrees in radians from -1 PI to 1 PI, but that has the same effect as 0 PI to 2 PI) between two vectors.
	// XMVector3AngleBetweenNormals returns an angle between two vectors, but always a positive result between
	// 0 and 1 PI. Which means, it doesn't tell us which half of the 2 PI degrees that are possible. So, we have the next if statement below,
	// which crosses the current characters direction and the worlds forward (0,0,1), which should give us the y axis vector (assuming that our character
	// rotates on the xz plane). We check to see if the y vector is positive ( > 0.0f), and if it is, we set the characters direction angle to be
	// the opposite of what it currently is, giving us the result in -1 PI to 1 PI.
	float charDirAngle = XMVectorGetX(XMVector3AngleBetweenNormals( XMVector3Normalize(currCharDirection), XMVector3Normalize(DefaultForward)));
	if(XMVectorGetY(XMVector3Cross(currCharDirection, DefaultForward)) > 0.0f)
		charDirAngle = -charDirAngle;

	// Now we update our characters position based off the frame time, his old position, and the direction he is facing
	float speed = 15.0f * time;
	charPosition = charPosition + (destinationDirection * speed);

	// Update characters world matrix
	XMMATRIX rotationMatrix;
	XMMATRIX Scale = XMMatrixScaling( 0.25f, 0.25f, 0.25f );
	XMMATRIX Translation = XMMatrixTranslation(XMVectorGetX(charPosition), 0.0f, XMVectorGetZ(charPosition) );
	rotationMatrix = XMMatrixRotationY(charDirAngle - 3.14159265f);		// Subtract PI from angle so the character doesn't run backwards

	worldMatrix = Scale * rotationMatrix * Translation;

	// Set the characters old direction
	oldCharDirection = currCharDirection;																

	// Update our animation
	float timeFactor = 1.0f;	// You can speed up or slow down time by changing this
	md5Model.UpdateMD5Model(pDeviceContext, md5Model.GetModel(), time*timeFactor, 0);
}

void D3DCamera::DetectInput(double time, HWND hWnd, MD5Model &md5Model, ID3D11DeviceContext *pDeviceContext)
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

	bool moveChar = false;
	XMVECTOR desiredCharDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	if(keyboardState[DIK_A] & 0x80)
	{
		desiredCharDir += (camRight);
		moveChar = true;
	}
	if(keyboardState[DIK_D] & 0x80)
	{
		desiredCharDir += -(camRight);
		moveChar = true;
	}
	if(keyboardState[DIK_W] & 0x80)
	{
		desiredCharDir += (camForward);
		moveChar = true;
	}
	if(keyboardState[DIK_S] & 0x80)
	{
		desiredCharDir += -(camForward);
		moveChar = true;
	}
	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.002f;

		camPitch += mouseCurrState.lY * 0.002f;
		// Check that the camera doesn't go over the top or under the player
		if(camPitch > 0.85f)
			camPitch = 0.85f;
		if(camPitch < -0.85f)
			camPitch = -0.85f;
		mouseLastState = mouseCurrState;
	}

	if(moveChar == true)
		MoveChar(time, desiredCharDir, playerCharWorld, md5Model, pDeviceContext);

	UpdateCamera();

	return;
}

void D3DCamera::UpdateCamera()
{	
	// Rotate target around camera
	/*camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);*/

	/*XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	// Walk
	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);
	camUp = XMVector3Cross(camForward, camRight);*/

	/*// Free Cam
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);*/

	/*camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;*/

	// Third Person Camera
	// Set the cameras target to be looking at the character.
	camTarget = charPosition;

	// This line is because this lessons model was set to stand on the point (0,0,0) (my bad), and we
	// don't want to just be looking at the models feet, so we move the camera's target vector up 5 units
	camTarget = XMVectorSetY(camTarget, XMVectorGetY(camTarget)+5.0f);	
    	
	// Unlike before, when we rotated the cameras target vector around the cameras position,
	// we are now rotating the cameras position around it's target (which is the character)
	// Rotate camera around target
	camRotationMatrix = XMMatrixRotationRollPitchYaw(-camPitch, camYaw, 0);
	camPosition = XMVector3TransformNormal(DefaultForward, camRotationMatrix );
	camPosition = XMVector3Normalize(camPosition);

	// Set our cameras position to rotate around the character. We need to add 5 to the characters
	// position's y axis because i'm stupid and modeled the character in the 3d modeling program
	// to be "standing" on (0,0,0), instead of centered around it ;) Well target her head here though
	camPosition = (camPosition * charCamDist) + camTarget;

	// We need to set our cameras forward and right vectors to lay
	// in the worlds xz plane, since they are the vectors we will
	// be using to determine the direction our character is running
	camForward = XMVector3Normalize(camTarget - camPosition);	// Get forward vector based on target
	camForward = XMVectorSetY(camForward, 0.0f);	// set forwards y component to 0 so it lays only on
	// the xz plane
	camForward = XMVector3Normalize(camForward);
	// To get our camera's Right vector, we set it's x component to the negative z component from the
	// camera's forward vector, and the z component to the camera forwards x component
	camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

	// Our camera does not "roll", so we can safely assume that the cameras right vector is always
	// in the xz plane, so to get the up vector, we just get the normalized vector from the camera
	// position to the cameras target, and cross it with the camera's Right vector
	camUp =XMVector3Normalize(XMVector3Cross(XMVector3Normalize(camPosition - camTarget), camRight));

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

}
