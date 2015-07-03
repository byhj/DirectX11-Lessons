
cbuffer cbPerObject :register(b0)
{
	float4x4 MVP;
};

struct VS_OUTPUT 
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUTPUT VS(float4 inPos: POSITION, float4 inColor: COLOR0)
{
   VS_OUTPUT vs_out;
   vs_out.Pos = mul(inPos, MVP);
   vs_out.Color = inColor;
 
   return vs_out;
}
