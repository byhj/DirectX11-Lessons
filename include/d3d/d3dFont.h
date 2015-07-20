#ifndef D3DFONT_H
#define D3DFONT_H

#include <FW1FontWrapper.h>
#pragma  comment(lib, "FW1FontWrapper.lib")

namespace byhj
{

class D3DFont
{
public:
	D3DFont() {};

    void Init(ID3D11Device *pD3D11Device)
	{
		HRESULT hr = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
		hr = pFW1Factory->CreateFontWrapper(pD3D11Device, L"Arial", &pFontWrapper);
		pFW1Factory->Release();
	}

	void drawText(ID3D11DeviceContext *pD3D11DeivceContext, WCHAR *text, 
		          float fontSize, float posX, float posY, float fontCoor);

	void drawFps(ID3D11DeviceContext *pD3D11DeviceContext, UINT fps);

private:
	IFW1Factory *pFW1Factory;
	IFW1FontWrapper *pFontWrapper;
};

}
#endif
