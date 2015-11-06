#ifndef INSTANCE_H
#define INSTANCE_H

#include <windows.h>
#include "d3d/model.h"
#include "d3d/Shader.h"
#include <DirectXMath.h> 

using namespace DirectX;

const int NumLeaves = 1000;
const int NumTrees  = 400;

namespace MeshStruct
{
	struct Vertex
	{

	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoords;
	XMFLOAT3 Tangent;
	XMFLOAT3 BiTangent;

	};
}

class Instance
{
public:
	Instance(){}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMFLOAT4X4 Model, XMFLOAT4X4  View, XMFLOAT4X4 Proj);
	void Shutdown() {}

	bool init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
	bool init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

private:

	struct MatrixBuffer
	{
		XMFLOAT4X4  model;
		XMFLOAT4X4  view;
		XMFLOAT4X4  proj;

	};
	MatrixBuffer cbMatrix;

	struct LeaveMatrixBuffer
	{
		XMMATRIX LeaveModel[NumLeaves];
	};
	LeaveMatrixBuffer cbLeaveMatrix;

	struct TreeMatrixBuffer
	{
		XMFLOAT4 TreeModel[NumTrees];
	};
	TreeMatrixBuffer cbTreeMatrix;

	struct InstanceBuffer
	{
		float isTree;
		float isLeaf;
		XMFLOAT2 padding;
	};
    InstanceBuffer cbInstance;

	struct LightBuffer
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT4 emissive;
	};
	LightBuffer cbLight;



	struct  Vertex
	{
		Vertex(float px, float py, float pz, float tu, float tv,
			  float nx, float ny, float nz)
			  :Position(px, py, pz), TexCoord(tu, tv), Normal(nx, ny, nz)
		{}

		XMFLOAT3 Position;
		XMFLOAT2 TexCoord;
		XMFLOAT3 Normal;
	};

	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11Buffer> m_pMVPBuffer;
	ComPtr<ID3D11Buffer> m_pLightBuffer;
	ComPtr<ID3D11Buffer> m_pLeaveMatrixBuffer;
	ComPtr<ID3D11Buffer> m_pTreeMatrixBuffer;
	ComPtr<ID3D11Buffer> m_pInstanceBuffer;
	ComPtr<ID3D11Buffer> m_pTreeVB;
	ComPtr<ID3D11Buffer> m_pTreeIB;
	ComPtr<ID3D11Buffer> m_pLeaveVB;
	ComPtr<ID3D11Buffer> m_pLeaveIB;

	ComPtr<ID3D11ShaderResourceView> m_pLeaveTexSRV;
	ComPtr<ID3D11ShaderResourceView> m_pTreeTexSRV;
	ComPtr<ID3D11SamplerState> m_pTexSamplerState;
	ComPtr<ID3D11InputLayout> m_pInputLayout;

	byhj::d3d::Model treeModel;
	byhj::d3d::Shader InstanceShader;
};



#endif
