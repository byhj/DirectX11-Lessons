#ifndef D3D_H
#define D3D_H

#include "d3dUtil.h"

class D3D11App
{
public:
	D3D11App(HINSTANCE hInstance);
	virtual ~D3D11App();

	HINSTANCE GetInstance() const;
	HWND      GetHwnd()     const;
	float     GetAspect()   const;

public:

	virtual int Run();
	virtual bool InitScene();
	void UpdateScene() {}
	virtual void RenderScene() {}
	virtual bool InitShader() { return true;}
	virtual bool InitBuffer() {return true;}
	virtual bool InitTexture() {return true;}
	virtual bool InitStatus() {return true;};

	//for draw text
	virtual bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter);
	virtual void RenderText(std::wstring text);
	void InitD2DScreenTexture();


	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//Mouse and Key 
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

protected:
	bool InitWindow();
	bool InitD3D();

protected:

	//Window hInstance
	HINSTANCE hInstance;
	HWND hWnd;
	bool Paused;
	HRESULT hr;

	//D3D common interface
	ID3D11Device              *pD3D11Device;
	ID3D11DeviceContext       *pD3D11DeviceContext;
	IDXGISwapChain            *pSwapChain;
	ID3D11Texture2D           *pDepthStencilBuffer;
	ID3D11RenderTargetView    *pRenderTargetView;
	ID3D11DepthStencilView    *pDepthStencilView;
	D3D11_VIEWPORT             Viewport;

	std::wstring WindowTitle;
	D3D_DRIVER_TYPE  DriverType;
	int ClientWidth;
	int ClientHeight;

	//Render text interface
	ID3D10Device1             *pD3D101Device;				   
	IDXGIKeyedMutex           *pkeyedMutex11;				   
	IDXGIKeyedMutex           *pkeyedMutex10;				   
	ID2D1RenderTarget         *pD2DRenderTarget;			   
	ID2D1SolidColorBrush      *pBrush;				   
	ID3D11Texture2D           *pBackBuffer11;				   
	ID3D11Texture2D           *pSharedTex11;				   
	ID3D11Buffer              *pD2DVertBuffer;				   
	ID3D11Buffer              *pD2DIndexBuffer;				   
	ID3D11ShaderResourceView  *pD2DTexture;		   
	IDWriteFactory            *pDWriteFactory;				   
	IDWriteTextFormat         *pTextFormat;	

	ID3D11BlendState          *d2dTransparency;
	ID3D11RasterizerState     *CCWcullMode;
	ID3D11RasterizerState     *CWcullMode;
	ID3D11SamplerState        *CubesTexSamplerState;
	ID3D11ShaderResourceView  *CubesTexture;
	std::wstring printText;
};



#endif