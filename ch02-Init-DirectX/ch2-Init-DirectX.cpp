#include <windows.h>
#include <d3d11.h>
#include <d3dX11.h>
#include <d3dX10.h>
#include <DxErr.h>
#include <xnamath.h>

#pragma  comment (lib, "d3d11.lib")
#pragma  comment (lib, "d3dx11.lib")
#pragma  comment (lib, "d3dx10.lib")
#pragma  comment (lib, "dxerr.lib")

IDXGISwapChain          *pSwapChain = NULL;
ID3D11Device            *pDevice = NULL;
ID3D11DeviceContext     *pDeviceContext = NULL;
ID3D11RenderTargetView  *pRenderTargetView = NULL;

LPCTSTR WndClassName = "DirectXWindow";

HWND hWnd  = NULL;
const int Width = 800;
const int Height = 600;
float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;

bool InitD3D(HINSTANCE hInstance);
bool InitScene();
void Release();
void UpdateScene();
void RenderScene();

bool InitWindow(HINSTANCE hInstance, int ShowCmd, int width, int height, bool windowed);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

   //Create buffer desc

   DXGI_MODE_DESC bufferDesc;
   ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
   bufferDesc.Width = Width;
   bufferDesc.Height = Height;
   bufferDesc.RefreshRate.Numerator = 60;
   bufferDesc.RefreshRate.Denominator = 1;
   bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
   bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

   //Create swapChain Desc

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

  

   //Create backbuffer
   ID3D11Texture2D *backBuffer;
   hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
   hr = pDevice->CreateRenderTargetView(backBuffer, NULL, &pRenderTargetView);
   backBuffer->Release();

   pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

   return true;
}

bool InitScene()
{
  return true;
}

void UpdateScene()
{
		//Update the colors of our scene
	red += colormodr * 0.00005f;
	green += colormodg * 0.00002f;
	blue += colormodb * 0.00001f;

	if(red >= 1.0f || red <= 0.0f)
		colormodr *= -1;
	if(green >= 1.0f || green <= 0.0f)
		colormodg *= -1;
	if(blue >= 1.0f || blue <= 0.0f)
		colormodb *= -1;
}

void Release()
{
   pSwapChain->Release();
   pDevice->Release();
   pDeviceContext->Release();
}

void RenderScene()
{
   D3DXCOLOR bgColor( red, green, blue, 1.0f );
   pDeviceContext->ClearRenderTargetView(pRenderTargetView, bgColor);
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