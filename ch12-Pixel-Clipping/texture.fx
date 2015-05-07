
cbuffer cbPerObject
{
	float4x4 MVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
    VS_OUTPUT output;

    output.Pos = mul(inPos, MVP);
    output.TexCoord = inTexCoord;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
   float4 diffuse = ObjTexture.Sample( ObjSamplerState, input.TexCoord );
    
	 //clip when the alpha is less than 0.25
    clip(diffuse.a - .25);
    
    return diffuse;
  
}
