#include "d3dApp.h"

//Vertex Structure and Vertex Layout (Input Layout)//
struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;
};
struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

Light light;

struct cbPerFrame
{
	Light  light;
};

cbPerFrame  cbPerLight;
cbPerObject cbPerObj;

int NumFaces = 0;
int NumVertices = 0;

struct HeightMapInfo{		// Heightmap structure
	int terrainWidth;		// Width of heightmap
	int terrainHeight;		// Height (Length) of heightmap
	XMFLOAT3 *heightMap;	// Array to store terrain's vertex positions
};

bool HeightMapLoad(char* filename, HeightMapInfo &hminfo);

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX Rotationx;
XMMATRIX Rotationz;

XMMATRIX WVP;
XMMATRIX camView;
XMMATRIX camProjection;

XMMATRIX d2dWorld;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

///////////////**************new**************////////////////////
XMVECTOR DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
///////////////**************new**************////////////////////

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

const int Width = 1000;
const int Height = 800;

//Fps Data and Function
double countsPerSecond = 0.0;
__int64 CounterStart = 0;
int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;
void StartTimer();
double GetTime();
double GetFrameTime();

class TextureApp : public D3D11App
{
public:
	TextureApp(HINSTANCE hInstance);
	~TextureApp();

	bool InitScene();
	bool InitBuffer();
	bool InitShader();
	bool InitD3D();
	bool InitStatus();
	bool InitTexture();
	int	 Run();
	void UpdateScene(double time);
	void RenderScene();
	void RenderText(std::wstring text, int inInt);
	void InitD2DScreenTexture();
	void UpdateCamera();
	bool InitDirectInput(HINSTANCE hInstance);
	void DetectInput(double time);

private:
	ID3D11Buffer* squareIndexBuffer;
	ID3D11Buffer* squareVertBuffer;

	ID3D11VertexShader    *pVS;
	ID3D11PixelShader     *pPS;
	ID3D10Blob            *pVS_Buffer;
	ID3D10Blob            *pPS_Buffer;
	ID3D11InputLayout     *pInputLayout;
	ID3D11Buffer          *cbPerObjectBuffer;
	ID3D11Buffer          *cbPerFrameBuffer;
	ID3D11PixelShader     *pD2D_PS;
	ID3D10Blob            *pD2D_PS_Buffer;
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;

	HRESULT hr;
};

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//init the window
	TextureApp app(hInstance);
	if ( !app.InitScene() )
		return 0;

	return  app.Run();
}

int TextureApp::Run()
{
	MSG msg = {0};

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
		{	
			frameCount++;
			if(GetTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}	
			frameTime = GetFrameTime();
			DetectInput(frameTime);
			UpdateScene(frameTime);

			RenderScene();
		}
	}

	return (int)msg.wParam;
}

TextureApp::TextureApp(HINSTANCE hInstance)
	:D3D11App(hInstance)
{
}

TextureApp::~TextureApp()
{
}
bool HeightMapLoad(char* filename, HeightMapInfo &hminfo)
{
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	// Open the file
	filePtr = fopen(filename,"rb");
	if (filePtr == NULL)
		return 0;

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1,filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;

	// Initialize the array which stores the image data
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k=0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	float heightFactor = 10.0f;

	// Read the image data into our heightMap array
	for(int j=0; j< hminfo.terrainHeight; j++)
	{
		for(int i=0; i< hminfo.terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = ( hminfo.terrainHeight * j) + i;

			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;

			k+=3;
		}
	}

	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}
void TextureApp::InitD2DScreenTexture()
{
	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),
	};


	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth      = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	pD3D11Device->CreateBuffer(&indexBufferDesc, &iinitData, &pD2DIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth      = sizeof( Vertex ) * 4;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = v;
	hr = pD3D11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &pD2DVertBuffer);


	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	pD3D11Device->CreateShaderResourceView(pSharedTex11, NULL, &pD2DTexture);

}
bool TextureApp::InitDirectInput(HINSTANCE hInstance)
{
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL); 

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	return true;
}

///////////////**************new**************////////////////////
void TextureApp::UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);

	// Free-Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;	

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
}

void TextureApp::DetectInput(double time)
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

	if(keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hWnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * time;

	if(keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if(keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if(keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if(keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;

		camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}

	UpdateCamera();

	return;
}
///////////////**************new**************////////////////////

bool TextureApp::InitScene()
{
	if(!D3D11App::InitWindow())
		return false;

	if(!D3D11App::InitD3D())
		return false;

	InitD2DScreenTexture();

	if(!InitShader())
		return false;

	if(!InitBuffer())
		return false;

	if(!InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitStatus())
		return false;

	if(!InitTexture())
		return false;



	return true;
}

bool TextureApp::InitBuffer()
{

	light.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	HeightMapInfo hmInfo;
	HeightMapLoad("heightmap.bmp", hmInfo);		// Load the heightmap and store it into hmInfo

	int cols = hmInfo.terrainWidth;
	int rows = hmInfo.terrainHeight;

	//Create the grid
	NumVertices = rows * cols;
	NumFaces  = (rows-1)*(cols-1)*2;

	std::vector<Vertex> v(NumVertices);

	for(DWORD i = 0; i < rows; ++i)
	{
		for(DWORD j = 0; j < cols; ++j)
		{
			v[i*cols+j].pos = hmInfo.heightMap[i*cols+j];
			v[i*cols+j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}

	std::vector<DWORD> indices(NumFaces * 3);

	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;
	for(DWORD i = 0; i < rows-1; i++)
	{
		for(DWORD j = 0; j < cols-1; j++)
		{
			indices[k]   = i*cols+j;		// Bottom left of quad
			v[i*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			indices[k+1] = i*cols+j+1;		// Bottom right of quad
			v[i*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k+2] = (i+1)*cols+j;	// Top left of quad
			v[(i+1)*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);


			indices[k+3] = (i+1)*cols+j;	// Top left of quad
			v[(i+1)*cols+j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k+4] = i*cols+j+1;		// Bottom right of quad
			v[i*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k+5] = (i+1)*cols+j+1;	// Top right of quad
			v[(i+1)*cols+j+1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

			k += 6; // next quad

			texUIndex++;
		}
		texUIndex = 0;
		texVIndex++;
	}

	//////////////////////Compute Normals///////////////////////////
	//Now we will compute the normals for each vertex using normal averaging
	std::vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Used to get vectors (sides) from the position of the verts
	float vecX, vecY, vecZ;

	//Two edges of our triangle
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//Compute face normals
	for(int i = 0; i < NumFaces; ++i)
	{
		//Get the vector describing one edge of our triangle (edge 0,2)
		vecX = v[indices[(i*3)]].pos.x - v[indices[(i*3)+2]].pos.x;
		vecY = v[indices[(i*3)]].pos.y - v[indices[(i*3)+2]].pos.y;
		vecZ = v[indices[(i*3)]].pos.z - v[indices[(i*3)+2]].pos.z;		
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

		//Get the vector describing another edge of our triangle (edge 2,1)
		vecX = v[indices[(i*3)+2]].pos.x - v[indices[(i*3)+1]].pos.x;
		vecY = v[indices[(i*3)+2]].pos.y - v[indices[(i*3)+1]].pos.y;
		vecZ = v[indices[(i*3)+2]].pos.z - v[indices[(i*3)+1]].pos.z;		
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

		//Cross multiply the two edge vectors to get the un-normalized face normal
		XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
		tempNormal.push_back(unnormalized);			//Save unormalized normal (for normal averaging)
	}

	//Compute vertex normals (normal Averaging)
	XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int facesUsing = 0;
	float tX;
	float tY;
	float tZ;

	//Go through each vertex
	for(int i = 0; i < NumVertices; ++i)
	{
		//Check which triangles use this vertex
		for(int j = 0; j < NumFaces; ++j)
		{
			if(indices[j*3] == i ||
				indices[(j*3)+1] == i ||
				indices[(j*3)+2] == i)
			{
				tX = XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

				normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum
				facesUsing++;
			}
		}

		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		normalSum = normalSum / facesUsing;

		//Normalize the normalSum vector
		normalSum = XMVector3Normalize(normalSum);

		//Store the normal in our current vertex
		v[i].normal.x = XMVectorGetX(normalSum);
		v[i].normal.y = XMVectorGetY(normalSum);
		v[i].normal.z = XMVectorGetZ(normalSum);

		//Clear normalSum and facesUsing for next vertex
		normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		facesUsing = 0;
	}
	/*************************************************************************************************/

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth      = sizeof(DWORD) * NumFaces * 3;
	indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = &indices[0];
	pD3D11Device->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumVertices;
	vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags      = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 
	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &v[0];
	hr = pD3D11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

	return true;
}

bool TextureApp::InitTexture()
{
	return true;
}

bool TextureApp::InitStatus()
{
	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width    = Width;
	viewport.Height   = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	pD3D11DeviceContext->RSSetViewports(1, &viewport);

	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage          = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth      = sizeof(cbPerObject);
	cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags      = 0;

	hr = pD3D11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage          = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth      = sizeof(cbPerFrame);
	cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags      = 0;
	hr                  = pD3D11Device->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

	//Camera information
	camPosition = XMVectorSet( 0.0f, 5.0f, -8.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
	camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)Width/Height, 1.0f, 1000.0f);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );

	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	HR( D3DX11CreateShaderResourceViewFromFile( pD3D11Device, L"../common/media/texture/grass.jpg",
		NULL, NULL, &CubesTexture, NULL ) );

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD         = 0;
	sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = pD3D11Device->CreateSamplerState( &sampDesc, &CubesTexSamplerState );

	pD3D11Device->CreateBlendState(&blendDesc, &d2dTransparency);

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = pD3D11Device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;

	hr = pD3D11Device->CreateRasterizerState(&cmdesc, &CWcullMode);

	return true;
}


///////////////**************new**************////////////////////
void StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}

double GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart-frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if(tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount)/countsPerSecond;
}


void TextureApp::UpdateScene(double time)
{
	//Reset cube1World
	groundWorld = XMMatrixIdentity();

	//Define cube1's world space matrix
	///////////////**************new**************////////////////////
	//Define terrains's world space matrix
	Scale = XMMatrixScaling( 10.0f, 10.0f, 10.0f );
	Translation = XMMatrixTranslation( -100.0f, -100.0f, -100.0f );

	//Set cube1's world space using the transformations
	groundWorld = Scale * Translation;
	///////////////**************new**************////////////////////

}

void TextureApp::RenderText(std::wstring text, int inInt)
{	
	pD3D11DeviceContext->PSSetShader(pD2D_PS, 0, 0);
	pkeyedMutex11->ReleaseSync(0);
	pkeyedMutex10->AcquireSync(0, 5);			

	pD2DRenderTarget->BeginDraw();	
	pD2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//Create our string
	std::wostringstream printString; 
	printString << text << inInt;;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	pBrush->SetColor(FontColor);	

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	//Draw the Text
	pD2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		pTextFormat,
		layoutRect,
		pBrush
		);

	pD2DRenderTarget->EndDraw();	

	pkeyedMutex10->ReleaseSync(1);
	pkeyedMutex11->AcquireSync(1, 5);

	pD3D11DeviceContext->OMSetBlendState(d2dTransparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	pD3D11DeviceContext->IASetIndexBuffer(pD2DIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &pD2DVertBuffer, &stride, &offset );

	WVP =  XMMatrixIdentity();
	cbPerObj.World = XMMatrixTranspose(WVP);
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	pD3D11DeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, &pD2DTexture );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	//pD3D11DeviceContext->RSSetState(CWcullMode);
	//Draw the second cube
	pD3D11DeviceContext->DrawIndexed( 6, 0, 0 );	
}


bool TextureApp::InitShader()
{
	//Compile Shaders from shader file
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &pVS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &pPS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"texture.fx", 0, 0, "D2D_PS", "ps_4_0", 0, 0, 0, &pD2D_PS_Buffer, 0, 0);

	//Create the Shader Objects
	hr = pD3D11Device->CreateVertexShader(pVS_Buffer->GetBufferPointer(), pVS_Buffer->GetBufferSize(), NULL, &pVS);
	hr = pD3D11Device->CreatePixelShader(pPS_Buffer->GetBufferPointer(), pPS_Buffer->GetBufferSize(), NULL, &pPS);
	hr = pD3D11Device->CreatePixelShader(pD2D_PS_Buffer->GetBufferPointer(), pD2D_PS_Buffer->GetBufferSize(), NULL, &pD2D_PS);

	//Set Vertex and Pixel Shaders
	pD3D11DeviceContext->VSSetShader(pVS, 0, 0);
	pD3D11DeviceContext->PSSetShader(pPS, 0, 0);


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = ARRAYSIZE(layout);
	hr = pD3D11Device->CreateInputLayout( layout, numElements, pVS_Buffer->GetBufferPointer(), 
		pVS_Buffer->GetBufferSize(), &pInputLayout );

	pD3D11DeviceContext->IASetInputLayout( pInputLayout );
	pD3D11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	return true;
}

void TextureApp::RenderScene()
{

	float bgColor[4] = {(0.0f, 0.0f, 0.0f, 0.0f)};
	pD3D11DeviceContext->ClearRenderTargetView(pRenderTargetView, bgColor);	
	pD3D11DeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	cbPerLight.light = light;
	pD3D11DeviceContext->UpdateSubresource( cbPerFrameBuffer, 0, NULL, &cbPerLight, 0, 0 );
	pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);	
	pD3D11DeviceContext->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );
	pD3D11DeviceContext->OMSetBlendState(0, 0, 0xffffffff);
	pD3D11DeviceContext->VSSetShader(pVS, 0, 0);
	pD3D11DeviceContext->PSSetShader(pPS, 0, 0);

	pD3D11DeviceContext->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the cubes vertex buffer
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &squareVertBuffer, &stride, &offset );

	WVP = groundWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);	
	cbPerObj.World = XMMatrixTranspose(groundWorld);
	pD3D11DeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	pD3D11DeviceContext->PSSetShaderResources( 0, 1, &CubesTexture );
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );
	pD3D11DeviceContext->RSSetState(CCWcullMode);
	pD3D11DeviceContext->DrawIndexed( NumFaces * 3, 0, 0 );


	RenderText(L"FPS: ", fps);

	//Present the backbuffer to the screen
	pSwapChain->Present(0, 0);
}