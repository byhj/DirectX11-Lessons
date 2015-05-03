#ifndef D3D_H
#define D3D_H

#include "d3dUtil.h"

class D3D11App
{
public:
	D3D11App(HINSTANCE hInstance);
	virtual ~D3D11App();

	HINSTANCE GetInstance() const;
	HWND      GetHwnd()     const;
	float     GetAspect()   const;

public:
	int Run();
	bool InitScene();
	virtual void UpdateScene() {}
	virtual void RenderScene() {}
	virtual bool InitShader() { return true;}
	virtual bool InitBuffer() {return true;}
	virtual bool InitTexture() {return true;}
	virtual bool InitD3D();
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//Mouse and Key 
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

protected:
	bool InitWindow();

protected:
	HINSTANCE hInstance;
	HWND hWnd;
	bool Paused;
	std::wstring WindowTitle;
	D3D_DRIVER_TYPE  DriverType;
	int ClientWidth;
	int ClientHeight;
};



namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	D3D11App* pD3D11App = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return pD3D11App->WndProc(hwnd, msg, wParam, lParam);
}

//initial the d3d status
D3D11App::D3D11App(HINSTANCE hInstance)
	:	hInstance(hInstance),
	WindowTitle(L"D3D11 Application"),
	DriverType(D3D_DRIVER_TYPE_HARDWARE),
	ClientWidth(1000),
	ClientHeight(800),
	hWnd(0),
	Paused(false)
{
	pD3D11App = this;
}

D3D11App::~D3D11App()
{
}

HINSTANCE D3D11App::GetInstance() const
{
	return  hInstance;
}

HWND D3D11App::GetHwnd()const
{
	return hWnd;
}

float D3D11App::GetAspect()const
{
	return static_cast<float>(ClientWidth) / static_cast<float>(ClientHeight);
}

int D3D11App::Run()
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
			UpdateScene();	
			RenderScene();
		}
	}

	return (int)msg.wParam;
}

bool D3D11App::InitScene()
{
	if(!InitWindow())
		return false;

	if(!InitD3D())
		return false;

	if(!InitBuffer())
		return false;

	if(!InitTexture())
		return false;

	if(!InitShader())
		return false;
	return true;
}


LRESULT CALLBACK D3D11App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (MessageBox(0, L"Are you sure to exit?", L"Really?",
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


bool D3D11App::InitWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	hWnd = CreateWindow(L"D3DWndClassName", WindowTitle.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0); 
	if( !hWnd )
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return true;
}

bool D3D11App::InitD3D()
{
	return true;
}

#endif