#ifndef D3DPICKING_H
#define D3DPICKING_H

#include <windows.h>
#include <xnamath.h>
#include <vector>

class D3DPicking
{
public:
	D3DPicking() 
	{
		m_pickRayPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		m_pickRayDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		m_ClientWidth  = 0;
		m_ClientHeight = 0;
	}
	~D3DPicking() {}

public:
	void InitPicking(int sw, int sh, std::vector<XMFLOAT3> vPos, std::vector<DWORD> vIndex)
	{
		m_ClientWidth = sw;
		m_ClientHeight = sh;
		m_PosData = vPos;
		m_IndexData = vIndex;
	}

	void PickRayVector(float mouseX, float mouseY, const XMMATRIX &camView, const XMMATRIX &camProj);

	float Pick(const XMMATRIX& worldSpace);

	bool PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point );

private:

	XMVECTOR m_pickRayPos;
	XMVECTOR m_pickRayDir;
	int m_ClientWidth;
	int m_ClientHeight;

	std::vector<XMFLOAT3> m_PosData;
	std::vector<DWORD> m_IndexData;
	
};	
#endif