#ifndef D3DAPP_H
#define D3DAPP_H

#include <string>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <windows.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "d3dDebug.h"

class D3DApp
{
public:
	D3DApp():m_AppName(L"Framework"), m_WndClassName(L"D3DWindow") 
	{

	}
	virtual ~D3DApp() {}

	int Run();
	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual	void v_Init();
	virtual bool v_Init();
	virtual void v_Reshape()  {}
	virtual void v_Shutdown() {}
	virtual void v_Render()   {}
	virtual void v_Update()   {}

	virtual void v_MouseDown() {}
	virtual void v_MouseMove() {}
	virtual void v_MouseUp()   {}

protected:

	int     m_ScreenWidth;
	int     m_ScreenHeight;
	float   m_ScreenFar;
	float   m_ScreenNear;
	int     m_PosX;
	int     m_PosY;
	LPCTSTR m_AppName;
	LPCTSTR m_WndClassName;

	//void      GetVideoCardInfo(char &, int &);
	HINSTANCE GetAppInst() const { return m_hInstance; }
	HWND      GetHwnd()    const { return m_hWnd; }
	float     GetAspect()  const { return (float)m_ScreenWidth / m_ScreenHeight; }

private:
	bool init_window();

private:
	HINSTANCE m_hInstance;
	HWND      m_hWnd;
	int       m_videoCardMemory;
	char      m_videoCardInfo;

};

//D3DApp * D3DApp::app; 
#define CALL_MAIN(a)                                    \
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) \
{                                                       \
	a *app = new a;                                     \
	app->Run();                                         \
	delete app;                                         \
	return 0;                                           \
}


#endif