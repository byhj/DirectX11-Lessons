#ifndef D3DFONT_H
#define D3DFONT_H

#include "d3dDebug.h"
#include <FW1FontWrapper.h>
#pragma  comment(lib, "FW1FontWrapper.lib")

class D3DFont
{
public:
	D3DFont() {};

    void init(ID3D11Device *pD3D11Device)
	{
		HRESULT hr = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
		hr = pFW1Factory->CreateFontWrapper(pD3D11Device, L"Arial", &pFontWrapper);
		DebugHR(hr);
		pFW1Factory->Release();
	}
	void drawText(ID3D11DeviceContext *pD3D11DeivceContext, WCHAR *text, 
		          float fontSize, float posX, float posY, float fontCoor);

	void drawFps(ID3D11DeviceContext *pD3D11DeviceContext, UINT fps);

private:
	IFW1Factory *pFW1Factory;
	IFW1FontWrapper *pFontWrapper;
};

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

#endif
