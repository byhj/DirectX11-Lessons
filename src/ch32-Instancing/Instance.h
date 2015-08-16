#ifndef INSTANCE_H
#define INSTANCE_H

#include <windows.h>
#include <d3dx11.h>
#include <xnamath.h>
#include "model.h"

#include "d3d/d3dShader.h"
#include "d3d/d3dDebug.h"

const int NumLeaves = 1000;
const int NumTrees  = 400;

class Instance
{
public:
	Instance()
	{
		m_pInputLayout        = NULL;
		m_pMVPBuffer          = NULL;
		m_pLightBuffer        = NULL;
		m_pTreeIB  = NULL;
		m_pTreeVB  = NULL;
		m_pLeaveIB = NULL;
		m_pLeaveVB = NULL;
		m_pInstanceBuffer = NULL;
		m_pLeaveMatrixBuffer = NULL;
		m_pTreeMatrixBuffer = NULL;
	}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMFLOAT4X4 Model, XMFLOAT4X4  View, XMFLOAT4X4 Proj);

	void shutdown()
	{
		ReleaseCOM(m_pRenderTargetView  )
		ReleaseCOM(m_pMVPBuffer         )
		ReleaseCOM(m_pLightBuffer       )
	}

	bool init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
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

	ID3D11RenderTargetView   *m_pRenderTargetView;
	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11Buffer             *m_pLightBuffer;
	ID3D11Buffer             *m_pLeaveMatrixBuffer;
	ID3D11Buffer             *m_pTreeMatrixBuffer;
	ID3D11Buffer             *m_pInstanceBuffer;
	ID3D11Buffer             *m_pTreeVB;
	ID3D11Buffer             *m_pTreeIB;
	ID3D11Buffer             *m_pLeaveVB;
	ID3D11Buffer             *m_pLeaveIB;

	ID3D11ShaderResourceView *m_pLeaveTexSRV;
	ID3D11ShaderResourceView *m_pTreeTexSRV;

	ID3D11SamplerState       *m_pTexSamplerState;
	ID3D11InputLayout        *m_pInputLayout;

	Model treeModel;
	Shader InstanceShader;
};



#endif
