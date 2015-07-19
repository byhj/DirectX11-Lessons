 
 struct MatrixType
 {
    float4x4 model;
	float4x4 view;
	float4x4 proj;
 };

cbuffer MatrixBuffer :register(b0)
{
	MatrixType g_Mat;
};

struct VS_IN
{
    float4 Pos : POSITION; //POSITION is same as POSITION0
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
   float4x4 mvp;
   mvp = mul(g_Mat.model, g_Mat.view);
   mvp = mul(mvp, g_Mat.proj);

   vs_out.Pos = mul(vs_in.Pos, mvp);
   vs_out.Color = vs_in.Color;
 
   return vs_out;
}
