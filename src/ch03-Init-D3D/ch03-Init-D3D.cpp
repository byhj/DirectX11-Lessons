#include "RenderSystem.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) 
{                                                   
	byhj::RenderSystem *app = new byhj::RenderSystem;                                 
	app->Run();                                     
	delete app;  

	return 0;                                       
}
