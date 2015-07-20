#include "d3dFont.h"

namespace byhj
{


void D3DFont::drawFps(ID3D11DeviceContext *pD3D11DeviceContext, UINT fps)
{
	static WCHAR frameStr[255];
	wsprintfW(frameStr, L"FPS: %u", fps);

	pFontWrapper->DrawString(
		pD3D11DeviceContext,
		frameStr,// String
		22.0f,// D3DFont size
		10.0f,// X position
		10.0f,// Y position
		0xff0099ff,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);
}

void D3DFont::drawText(ID3D11DeviceContext *pD3D11DeivceContext, WCHAR *text, 
					   float fontSize, float posX, float posY, float fontCoor)
{

	pFontWrapper->DrawString(
		pD3D11DeivceContext,
		text,// String
		fontSize,// D3DFont size
		posX,// X position
		posY,// Y position
		0xff0099ff,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);
}


}