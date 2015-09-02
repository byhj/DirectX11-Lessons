#include "RenderSystem.h"

                            
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) 
{                                                   
	auto app = new byhj::RenderSystem;                                 
	app->Run();                                     
	delete app;  

	return 0;                                       
}