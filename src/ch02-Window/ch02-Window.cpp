#include "d3d/d3dApp.h"

class FrameWorkApp: public D3DApp
{
public:
	FrameWorkApp()
	{
		 m_AppName = L"DirectX11.0: ch02-FrameWork";
	}
	~FrameWorkApp(){}
};

CALL_MAIN(FrameWorkApp);