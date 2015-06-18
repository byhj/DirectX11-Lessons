 
cbuffer MatrixBuffer
{
	float4x4 MVP;
};

struct VS_IN
{
    float4 Pos : POSITION;
    float4 Color : COLOR0;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUT VS( VS_IN vs_in )
{	
 
   VS_OUT vs_out;
   vs_out.Pos = mul(vs_in.Pos, MVP);
   vs_out.Color = vs_in.Color;
 
   return vs_out;
}
