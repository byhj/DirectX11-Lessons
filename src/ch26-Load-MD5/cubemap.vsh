cbuffer MatrixBuffer
{
  float4x4 MVP;
};

struct VS_IN
{
  float3 Pos: POSITION;
};

struct VS_OUT
{
  float4 Pos: SV_POSITION;
  float3 Tex: TEXCOORD;
};

VS_OUT VS(VS_IN vs_in)
{
    VS_OUT vs_out;

	//w=1.0f, make sure our skymap is always the furthest object in our scene
    vs_out.Pos = mul(float4(vs_in.Pos, 1.0f), MVP).xyww;
	vs_out.Tex = vs_in.Pos;

	return vs_out;
}