#ifndef D3DSHADER_H
#define D3DSHADER_H

#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>
#include <iostream>
#include <string>

class Shader
{
public:
	Shader() {}
	~Shader() {}

	void init(ID3D11Device *pD3D11Device, HWND hWnd);
	bool attachVS(WCHAR* Filename,  D3D11_INPUT_ELEMENT_DESC *pInputLayoutDesc, unsigned numElements);
	bool attachPS(WCHAR* Filename);
	void use(ID3D11DeviceContext *pD3D111DeviceContext);
	void Debug(ID3D10Blob *pErrorMessage, HWND hwnd, WCHAR *shaderFileName);
	void end();

private:
	ID3D11VertexShader *pVS_Shader;
	ID3D11PixelShader  *pPS_Shader;
	ID3D11InputLayout  *pInputLayout;

	ID3D11Device *pD3D11Device;
	HWND hWnd;
};


#endif