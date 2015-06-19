
cbuffer cbPerObject
{
	float4x4 MVP;
};

struct VS_OUTPUT 
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT VS(float4 inPos: POSITION, float4 inColor: COLOR)
{
   VS_OUTPUT vs_out;
   vs_out.Pos = mul(inPos, MVP);
   vs_out.Color = inColor;
 
   return vs_out;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
   return  input.Color;
}