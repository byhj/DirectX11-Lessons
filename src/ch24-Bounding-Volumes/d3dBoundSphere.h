#ifndef D3DBOUNDSPHERE_H
#define D3DBOUNDSPHERE_H

#include <windows.h>
#include <xnamath.h>
#include <vector>

class D3DBoundSphere
{
public:
	D3DBoundSphere() {}
	~D3DBoundSphere() {}

	void CreateBuondSphere(const std::vector<XMFLOAT3> &vPosData);

private:

	float    boundingSphere;
	XMVECTOR objectCenterOffset;
};
#endif
