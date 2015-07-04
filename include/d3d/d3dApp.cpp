#include "d3dApp.h"

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static D3DApp *AppHandle = 0;

int D3DApp::Run()
{	
	v_Init();

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
			v_Render();
		}

	}
	v_Shutdown();
	return (int)msg.wParam;
}

bool D3DApp::v_Init()
{
	if (!init_window())
		return false;

	if (!v_InitD3D())
		return false;

	return true;
}


bool D3DApp::init_window()
{
	//Set the window in the middle of screen
	m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN) * 0.75;
	m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN) * 0.75;
	m_PosX = (GetSystemMetrics(SM_CXSCREEN) - m_ScreenWidth)  / 2;
	m_PosY = (GetSystemMetrics(SM_CYSCREEN) - m_ScreenHeight) / 2;
	m_ScreenNear = 0.1f;
	m_ScreenFar  = 1000.0f;

	AppHandle = this;
	m_hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wc;	
	wc.cbSize = sizeof(WNDCLASSEX);	
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;	
	wc.cbClsExtra = NULL;	
	wc.cbWndExtra = NULL;	
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);	
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;	
	wc.lpszClassName = m_WndClassName;	
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Registering Class Failded",	L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	//Create the window and show

	m_hWnd = CreateWindowEx(	
		NULL,	           
		m_WndClassName,
		m_AppName,
		WS_OVERLAPPEDWINDOW,	
		m_PosX, m_PosY,
		m_ScreenWidth,	
		m_ScreenHeight,	
		NULL,
		NULL,
		m_hInstance,	
		NULL
		);

	if (!m_hWnd )	
	{
		MessageBox(NULL, L"Creating Window Failed", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);	

	return true;
}

bool D3DApp::v_InitD3D()
{
	return true;
}

LRESULT CALLBACK D3DApp::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	} 

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}
		// All other messages pass to the message handler in the system class.
	default:
		{
			return AppHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
