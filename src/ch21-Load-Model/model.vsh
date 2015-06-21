 
cbuffer MatrixBuffer
{
	float4x4 MVP;
};

struct VS_IN
{
    float4 Pos   : POSITION;
	float3 Normal: NORMAL;
    float2 Tex   : TEXCOORD0;
};

struct VS_OUT
{
    float4 Pos   : SV_POSITION;
	float3 Normal: NORMAL;
    float2 Tex   : TEXCOORD0;
	float3 worldPos : POSITION;
};

VS_OUT VS( VS_IN vs_in )
{	
 
   VS_OUT vs_out;

   vs_out.Pos = mul(vs_in.Pos, MVP);
   vs_out.Normal = mul(vs_in.Normal, MVP);
   vs_out.worldPos = vs_out.Pos;
   vs_out.Tex = vs_in.Tex;
 
   return vs_out;
}
