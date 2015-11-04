#ifndef MD5MODEL_H
#define MD5MODEL_H

#include <string>
#include <fstream>
#include <istream>
#include <vector>

#include <d3dx11.h>
#include <DirectXMath.h> using namespace DirectX;

#include "d3d/d3dDebug.h"
#include "d3d/Shader.h"


#pragma region DataStruct

struct VertexType	//Overloaded VertexType Structure
{
	VertexType(){}
	VertexType(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 biTangent;

	// Will not be sent to shader
	int StartWeight;
	int WeightCount;
};

struct Joint
{
	std::wstring name;
	int parentID;

	XMFLOAT3 pos;
	XMFLOAT4 orientation;
};


///////////////**************new**************////////////////////
struct BoundingBox
{
	XMFLOAT3 min;
	XMFLOAT3 max;
};

struct FrameData
{
	int frameID;
	std::vector<float> frameData;
};
struct AnimJointInfo
{
	std::wstring name;
	int parentID;

	int flags;
	int startIndex;
};


struct ModelAnimation
{
	int numFrames;
	int numJoints;
	int frameRate;
	int numAnimatedComponents;

	float frameTime;
	float totalAnimTime;
	float currAnimTime;

	std::vector<AnimJointInfo> jointInfo;
	std::vector<BoundingBox> frameBounds;
	std::vector<Joint>	baseFrameJoints;
	std::vector<FrameData>	frameData;
	std::vector<std::vector<Joint>> frameSkeleton;
};

struct Weight
{
	int jointID;
	float bias;
	XMFLOAT3 pos;
	///////////////**************new**************////////////////////
	XMFLOAT3 normal;
	///////////////**************new**************////////////////////
};//Create material structure

struct SurfaceMaterial
{
	std::wstring matName;
	XMFLOAT4 difColor;
	int texArrayIndex;
	int normMapTexArrayIndex;
	bool hasNormMap;
	bool hasTexture;
	bool transparent;
};


struct ModelSubset
{
	int texArrayIndex;
	int numTriangles;

	std::vector<VertexType> vertices;
	std::vector<XMFLOAT3> jointSpaceNormals;
	std::vector<DWORD> indices;
	std::vector<Weight> weights;
	std::vector<XMFLOAT3> positions;

	ID3D11Buffer* vertBuff; 
	ID3D11Buffer* indexBuff;
};

struct Model3D
{
	int numSubsets;
	int numJoints;

	std::vector<Joint> joints;
	std::vector<ModelSubset> subsets;
	std::vector<ModelAnimation> animations;
};



#pragma endregion

class MD5Model
{
public:
	MD5Model()
	{
		m_pInputLayout        = NULL;
		m_pMVPBuffer          = NULL;
		m_pLightBuffer        = NULL;
		m_pVertexBuffer       = NULL;
		m_pIndexBuffer        = NULL;
	}
	~MD5Model() {}

public:

	void Render(ID3D11DeviceContext *pD3D11DeviceContext,  XMFLOAT4X4 Model, XMFLOAT4X4 View, XMFLOAT4X4 Proj);
	bool init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);
	void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);

	bool LoadMD5Model(ID3D11Device *pD3D11Device, std::wstring filename, Model3D& MD5Model,
		std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
		std::vector<std::wstring> texFileNameArray);
	bool LoadMD5Anim(std::wstring filename,	Model3D& MD5Model);
	void UpdateMD5Model(ID3D11DeviceContext *pD3D11DeviceContext, Model3D& MD5Model, float deltaTime, int animation);
	Model3D & GetModel()
	{
		return NewMD5Model;
	}
private:
	struct MatrixBuffer
	{
		XMFLOAT4X4  model;
		XMFLOAT4X4  view;
		XMFLOAT4X4  proj;

	};
	MatrixBuffer cbMatrix;

	struct LightBuffer
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		FLOAT padding;
	};
	LightBuffer cbLight;

	ID3D11RenderTargetView   *m_pRenderTargetView;
	ComPtr<ID3D11Buffer> m_pMVPBuffer;
	ComPtr<ID3D11Buffer> m_pLightBuffer;
	ID3D11Buffer             *m_CameraBuffer;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	std::vector<ID3D11ShaderResourceView*> m_pTexture;
	ComPtr<ID3D11SamplerState> m_pTexSamplerState;
	ID3D11InputLayout        *m_pInputLayout;

	std::vector<SurfaceMaterial> material;
	Model3D NewMD5Model;
	std::vector<std::wstring> texFile;

	int m_VertexCount = 0;
	int m_IndexCount = 0;
	std::vector<VertexType> VertexData;
	std::vector<unsigned int> IndexData;

    byhj::d3d::Shader CubeShader;
};
#endif