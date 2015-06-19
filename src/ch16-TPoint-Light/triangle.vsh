 
cbuffer MatrixBuffer
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};

struct VS_IN
{
    float4 Pos   : POSITION;
    float2 Tex   : TEXCOORD0;
	float3 Normal: NORMAL;
};

struct VS_OUT
{
    float4 Pos      : SV_POSITION;
    float2 Tex      : TEXCOORD0;
	float3 Normal   : NORMAL;
	float3 worldPos : POSITION;
};

VS_OUT VS( VS_IN vs_in )
{	
 
   VS_OUT vs_out;
   vs_out.Pos = vs_in.Pos;
   vs_out.Pos = mul(vs_in.Pos, model);
   vs_out.worldPos= vs_out.Pos;

   vs_out.Pos = mul(vs_out.Pos, view);
   vs_out.Pos = mul(vs_out.Pos, proj);
  
   vs_out.Normal = mul(vs_in.Normal, model);
   vs_out.Tex = vs_in.Tex;
 
   return vs_out;
}
