#include <windows.h>
#include <d3d11.h>
#include <d3dX11.h>
#include <d3dX10.h>
#include <xnamath.h>

#pragma  comment (lib, "d3d11.lib")
#pragma  comment (lib, "d3dx11.lib")
#pragma  comment (lib, "d3dx10.lib")

IDXGISwapChain          *pSwapChain = NULL;
ID3D11Device            *pDevice = NULL;
ID3D11DeviceContext     *pDeviceContext = NULL;
ID3D11RenderTargetView  *pRenderTargetView = NULL;
ID3D11Buffer *pBuffer = NULL;
ID3D11VertexShader *pVS;
ID3D11PixelShader  *pPS;
ID3DBlob *VS_Buffer;
ID3DBlob *PS_Buffer;
ID3D11InputLayout *pInputLayout;


LPCTSTR WndClassName = "DirectXWindow";
HWND hWnd  = NULL;
const int Width = 1000;
const int Height = 800;

bool InitD3D(HINSTANCE hInstance);
bool InitScene();
void Release();
void UpdateScene();
void RenderScene();
bool InitWindow(HINSTANCE hInstance, int ShowCmd, int width, int height, bool windowed);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Vertex {
	Vertex(){}
	Vertex(float x, float y, float z)
		:Pos(x, y, z) {}

  XMFLOAT3 Pos;
};

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{

    if(!InitWindow(hInstance, nShowCmd, Width, Height, true))
	 {
	    MessageBox(0, "Window Initialization - Failed", "Error", MB_OK);
	    return 0;
	 } 

    if (!InitD3D(hInstance) )
    {
       MessageBox(0, "Init D3D Failed", "Error", MB_OK);
	   return 0;
    }

    if (!InitScene())
    {
      MessageBox(0, "Init Scene Failed", "Error", MB_OK);
      return 0;
    }

	 MSG msg;
	 ZeroMemory(&msg, sizeof(MSG));
	 while (true)
	{
		 if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			   if(msg.message == WM_QUIT)
				   break;
			   TranslateMessage(&msg);
			   DispatchMessage(&msg);
		 }
		 else 
		{
		    UpdateScene();
			RenderScene();
		}

	}
	   return (int)msg.wParam;
}

bool InitD3D(HINSTANCE hInstance)
{
   HRESULT hr;

   //create buffer desc
   DXGI_MODE_DESC bufferDesc;
   ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
   bufferDesc.Width = Width;
   bufferDesc.Height = Height;
   bufferDesc.RefreshRate.Numerator = 60;
   bufferDesc.RefreshRate.Denominator = 1;
   bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
   bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

   //create swapChain Desc
   DXGI_SWAP_CHAIN_DESC swapChainDesc;
   ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
   swapChainDesc.BufferDesc = bufferDesc;
   swapChainDesc.SampleDesc.Count = 1;
   swapChainDesc.SampleDesc.Quality = 0;
   swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swapChainDesc.BufferCount = 1;
   swapChainDesc.OutputWindow = hWnd;
   swapChainDesc.Windowed = TRUE;
   swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

   hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
	                                  NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, 
	                                  &swapChainDesc, &pSwapChain, &pDevice,
	                                  NULL, &pDeviceContext);

   //create backbuffer
   ID3D11Texture2D *backBuffer;
   hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
   hr = pDevice->CreateRenderTargetView(backBuffer, NULL, &pRenderTargetView);
   backBuffer->Release();

   pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

   return true;
}

bool InitScene()
{
   HRESULT hr;
   hr = D3DX11CompileFromFile("triangle.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &VS_Buffer, 0, 0);
   hr = D3DX11CompileFromFile("triangle.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &PS_Buffer, 0, 0);
   hr = pDevice->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(),NULL, &pVS);
   hr = pDevice->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(),NULL, &pPS);
  
   pDeviceContext->VSSetShader(pVS, 0, 0);
   pDeviceContext->PSSetShader(pPS, 0, 0);

   //triangle data
   Vertex VertexData[] = 
   {
	   Vertex( 0.0f, 0.5f, 0.5f ),
	   Vertex( 0.5f, -0.5f, 0.5f ),
	   Vertex( -0.5f, -0.5f, 0.5f ),
   }; 

   D3D11_BUFFER_DESC vboDesc;
   ZeroMemory(&vboDesc, sizeof(vboDesc));
   vboDesc.Usage = D3D11_USAGE_DEFAULT;
   vboDesc.ByteWidth = sizeof(Vertex) * 3;
   vboDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   vboDesc.CPUAccessFlags = 0;
   vboDesc.MiscFlags = 0;

   //set data to vbo
   D3D11_SUBRESOURCE_DATA vbo;
   ZeroMemory(&vbo, sizeof(vbo));
   vbo.pSysMem = VertexData;
   hr = pDevice->CreateBuffer(&vboDesc, &vbo, &pBuffer);
   UINT stride = sizeof(Vertex);
   UINT offset = 0;
   pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);

   //set input layout
   D3D11_INPUT_ELEMENT_DESC layout[] =
   {
		   { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
   };
   UINT numElements = ARRAYSIZE(layout);
   pDevice->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &pInputLayout);
   pDeviceContext->IASetInputLayout(pInputLayout);
   pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

   //set viewpot
   D3D11_VIEWPORT vp;
   ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
   vp.TopLeftX = 0;
   vp.TopLeftY = 0;
   vp.Width = Width;
   vp.Height = Height;
   pDeviceContext->RSSetViewports(1, &vp);

   return true;
}

void UpdateScene()
{

}
void Release()
{
   pSwapChain->Release();
   pDevice->Release();
   pDeviceContext->Release();
   pBuffer->Release();
   pVS->Release();
   pPS->Release();
   pInputLayout->Release();
   VS_Buffer->Release();
   PS_Buffer->Release();
}

void RenderScene()
{
   D3DXCOLOR bgColor( 0.0, 0.0, 0.0, 1.0f );
   pDeviceContext->ClearRenderTargetView(pRenderTargetView, bgColor);

   pDeviceContext->Draw(3, 0);

   pSwapChain->Present(0, 0);
}

bool InitWindow(HINSTANCE hInstance, int ShowCmd, int width, int height, bool windowed)
{

	WNDCLASSEX wc;	
	wc.cbSize = sizeof(WNDCLASSEX);	
	wc.style = CS_HREDRAW | CS_VREDRAW;	
	wc.lpfnWndProc = WndProc;	
	wc.cbClsExtra = NULL;	
	wc.cbWndExtra = NULL;	
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);	
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;	
	wc.lpszClassName = WndClassName;	
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 

	if (!RegisterClassEx(&wc))
	{
    		
		MessageBox(NULL, "Registering Class Failded",	"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hWnd = CreateWindowEx(	
		NULL,	           
		WndClassName,	  
		"DirectX11-Init", 
		WS_OVERLAPPEDWINDOW,	
		300, 100,
		width,	
		height,	
		NULL,
		NULL,
		hInstance,	
		NULL
		);

	if (!hWnd )	
	{
		MessageBox(NULL, "Creating Window Failed", "Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hWnd, ShowCmd);
	UpdateWindow(hWnd);	

	return true;	
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
	{
      case WM_KEYDOWN:
		  if (wParam == VK_ESCAPE)
		  {
		     if (MessageBox(0, "Are you sure to exit?", "Really?",
				            MB_YESNO | MB_ICONQUESTION) == IDYES)
             DestroyWindow(hWnd);
		  }
		  return 0;

	  case WM_DESTROY:
		PostQuitMessage(0);
	    return 0;

	  default:
		  break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}