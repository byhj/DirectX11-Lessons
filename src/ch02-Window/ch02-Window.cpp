#include "RenderSystem.h"
#include <memory>
                            
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) 
{                                                   
	auto app = std::make_shared<byhj::RenderSystem>();
	app->Run();

	return 0;                                       
}