#include "RenderSystem.h"

                            
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) 
{                                                   
	RenderSystem *app = new RenderSystem;                                 
	app->Run();                                     
	delete app;  

	return 0;                                       
}