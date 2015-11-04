#ifndef D3DAABB_H
#define D3DAABB_H

#include <windows.h>
#include <DirectXMath.h> using namespace DirectX;
#include <vector>

class D3DAABB
{
public:
	D3DAABB() {}
	~D3DAABB() {}

public:

	// Culling Functions
	void CreateAABB(std::vector<XMFLOAT3> &vertPosArray);
	void CreateFrustumPlanes(const XMMATRIX& viewProj);
	bool CullAABB(XMFLOAT4 worldPos);

private:
	XMFLOAT3 m_AABB[2];
	XMFLOAT4 m_FrustumPlanes[6];

};
#endif