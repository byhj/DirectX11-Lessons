#include <windows.h>

LPCTSTR WndClassName = "DirectXWindow";
HWND hWnd  = NULL;

const int Width = 800;
const int Height = 600;

bool InitWindow(HINSTANCE hInstance, int ShowCmd, int width, int height, bool windowed);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
   if (!InitWindow(hInstance, nShowCmd, Width, Height, true) )
   {
       MessageBox(0, "Init Window Failed", "Error", MB_OK);
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
			   //render();
		 }

	}
	   return (int)msg.wParam;
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
		"DirectX11-Window", 
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