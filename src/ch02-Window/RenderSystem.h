#include "d3d/App.h"

namespace byhj
{

class RenderSystem: public d3d::App
{
public:
	RenderSystem()
	{
		m_AppName = L"DirectX11: ch02-Window";
	}

	~RenderSystem(){}

public:
	void v_Init();
	void v_Update();
	void v_Render();
	void v_Shutdown();
};

}