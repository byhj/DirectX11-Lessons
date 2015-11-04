
#ifndef INSTANCE_H
#define INSTANCE_H

#include <windows.h>
#include <d3dx11.h>
#include <DirectXMath.h> using namespace DirectX;
#include "model.h"

#include "d3d/Shader.h"
#include "d3d/d3dDebug.h"
#include "d3d/d3dAABB.h"

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
		m_pTreeIB             = NULL;
		m_pTreeVB             = NULL;
		m_pLeaveIB            = NULL;
		m_pLeaveVB            = NULL;
		m_pInstanceBuffer     = NULL;
		m_pLeaveMatrixBuffer  = NULL;
		m_pTreeMatrixBuffer   = NULL;
		m_CullLeftNum         = 0;
	}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMFLOAT4X4 Model,  XMFLOAT4X4  View, XMFLOAT4X4 Proj, const XMMATRIX &viewProj)
	{	

		unsigned int stride;
		unsigned int offset;

		m_CullLeftNum = 0;
		m_D3DAABB.CreateAABB(treeModel.GetPos());	
		m_D3DAABB.CreateFrustumPlanes(viewProj);

		XMFLOAT4 TreecullModel[NumTrees];
		int j = 0;
		for (int i = 0; i != NumTrees; ++i)
		{
			bool isCull = m_D3DAABB.CullAABB( cbTreeMatrix.TreeModel[i]);
			if (!isCull)
			{
				TreecullModel[j++] = cbTreeMatrix.TreeModel[i];
				++m_CullLeftNum;
			}
		}

		pD3D11DeviceContext->UpdateSubresource(m_pTreeMatrixBuffer, 0, NULL, &TreecullModel, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers(3, 1, &m_pTreeMatrixBuffer);

		cbMatrix.model  = Model;
		cbMatrix.view   = View;
		cbMatrix.proj   = Proj;
		pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, m_pMVPBuffer.GetAddressOf());
		pD3D11DeviceContext->VSSetConstantBuffers(2, 1, &m_pLeaveMatrixBuffer);

		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pD3D11DeviceContext->PSSetSamplers( 0, 1, m_pTexSamplerState.GetAddressOf());

		InstanceShader.use(pD3D11DeviceContext);

		///////////////////////////////////////////////////////////////////////////////////
		stride = sizeof(MeshStruct::Vertex); 
		offset = 0;
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pLeaveVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pLeaveIB, DXGI_FORMAT_R32_UINT, 0);
		pD3D11DeviceContext->PSSetShaderResources( 0, 1, &m_pLeaveTexSRV);

		cbInstance.isTree = 0.0f;
		cbInstance.isLeaf = 2.0f;
		pD3D11DeviceContext->UpdateSubresource(m_pInstanceBuffer, 0, NULL, &cbInstance, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers(1, 1, &m_pInstanceBuffer);
		pD3D11DeviceContext->DrawIndexedInstanced(6, m_CullLeftNum * NumLeaves, 0, 0, 0);
		///////////////////////////////////////////////////////////////////////////////////
		stride = sizeof(MeshStruct::Vertex); 
		offset = 0;
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pTreeVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pTreeIB, DXGI_FORMAT_R32_UINT, 0);
		pD3D11DeviceContext->PSSetShaderResources( 0, 1, &m_pTreeTexSRV);

		cbInstance.isTree = 2.0f;
		cbInstance.isLeaf = 0.0f;
		pD3D11DeviceContext->UpdateSubresource(m_pInstanceBuffer, 0, NULL, &cbInstance, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers(1, 1, &m_pInstanceBuffer);
		pD3D11DeviceContext->DrawIndexedInstanced(treeModel.GetIndexCount(), m_CullLeftNum, 0, 0, 0);
	}

	void shutdown()
	{
		ReleaseCOM(m_pRenderTargetView  )
			ReleaseCOM(m_pMVPBuffer         )
			ReleaseCOM(m_pLightBuffer       )
	}

	bool init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	bool init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

	int GetCullLeft() const
	{
       return m_CullLeftNum;
	}

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
	ComPtr<ID3D11Buffer> m_pMVPBuffer;
	ComPtr<ID3D11Buffer> m_pLightBuffer;
	ID3D11Buffer             *m_pLeaveMatrixBuffer;
	ID3D11Buffer             *m_pTreeMatrixBuffer;
	ID3D11Buffer             *m_pInstanceBuffer;
	ID3D11Buffer             *m_pTreeVB;
	ID3D11Buffer             *m_pTreeIB;
	ID3D11Buffer             *m_pLeaveVB;
	ID3D11Buffer             *m_pLeaveIB;

	ID3D11ShaderResourceView *m_pLeaveTexSRV;
	ID3D11ShaderResourceView *m_pTreeTexSRV;

	ComPtr<ID3D11SamplerState> m_pTexSamplerState;
	ID3D11InputLayout        *m_pInputLayout;

	Model treeModel;
	Shader InstanceShader;
	int m_CullLeftNum;
	D3DAABB m_D3DAABB;
};



#endif
