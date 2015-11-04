#ifndef D3DBOUNDBOX_H
#define D3DBOUNDBOX_H

#include <windows.h>
#include <DirectXMath.h> using namespace DirectX;
#include <vector>

class D3DBoundBox
{
public:
	D3DBoundBox(){}
	~D3DBoundBox() {}

public:

	void CreateBoundBox(const std::vector<XMFLOAT3> &vPosData);

	std::vector<XMFLOAT3> GetPosData();
	std::vector<DWORD> GetIndexData();
	XMFLOAT3 GetMinVertex();
	XMFLOAT3 GetMaxVertex();

private:

	std::vector<XMFLOAT3> m_BoxVertexData;
	std::vector<DWORD>    m_BoxIndexData;
	XMFLOAT3 m_MinVertex;
	XMFLOAT3 m_MaxVertex;
};

#endif