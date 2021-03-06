#include "d3dShader.h"

void Shader::init(ID3D11Device *pD3D11Device, HWND hWnd)
{
	this->pD3D11Device = pD3D11Device;
	this->hWnd = hWnd;
}

void Shader::Debug(ID3D10Blob *pErrorMessage, HWND hwnd, WCHAR *shaderFileName)
{
	char *pCompileErrors;
	unsigned long bufferSize, i;

	pCompileErrors = (char*)(pErrorMessage->GetBufferPointer());
	std::string errorStr = pCompileErrors;
	//unsigned found = errorStr.find_last_of("\\") ;
	//errorStr = errorStr.substr( found + 1);
	std::cerr << pCompileErrors;

	pErrorMessage->Release();
	pErrorMessage = 0;

	//MessageBox(hwnd, L"Error compiling shader", shaderFileName, MB_OK);

	//return;
	// Pop a message up on the screen to notify the user to check the text file for compile errors.

}

bool Shader::attachVS(WCHAR* Filename,  std::vector<D3D11_INPUT_ELEMENT_DESC> vInputLayoutDesc)
{	
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* VertexShaderBuffer = 0;

	result = D3DX11CompileFromFile(Filename, NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&VertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		if(errorMessage)
			Debug(errorMessage, hWnd, Filename);
		else
			MessageBox(hWnd, Filename, L"Can not open Vertex Shader File", MB_OK);
	}


	// Create the vertex shader from the buffer.
	result = pD3D11Device->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), NULL, &pVS_Shader);
	if(FAILED(result))
	{
		return false;
	}
	auto numElements = vInputLayoutDesc.size();
	result = pD3D11Device->CreateInputLayout(&vInputLayoutDesc[0], numElements, VertexShaderBuffer->GetBufferPointer(), 
		                                     VertexShaderBuffer->GetBufferSize(), &pInputLayout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	VertexShaderBuffer->Release();
	VertexShaderBuffer = 0;

	return true;
}

bool Shader::attachPS(WCHAR* Filename)
{	
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* PixelShaderBuffer = 0;

	result = D3DX11CompileFromFile(Filename, NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&PixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		if(errorMessage)
			Debug(errorMessage, hWnd, Filename);
		else
			MessageBox(hWnd, Filename, L"Can not open Pixel Shader File", MB_OK);
	}

	// Create the vertex shader from the buffer.
	result = pD3D11Device->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &pPS_Shader);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	PixelShaderBuffer->Release();
	PixelShaderBuffer = 0;

	return true;
}

void Shader::use(ID3D11DeviceContext *pD3D11DeviceContext)
{
	pD3D11DeviceContext->IASetInputLayout(pInputLayout);
	pD3D11DeviceContext->VSSetShader(pVS_Shader, NULL, 0);
	pD3D11DeviceContext->PSSetShader(pPS_Shader, NULL, 0);
}

void Shader::end()
{
	pD3D11Device = 0;
	hWnd = 0;
}