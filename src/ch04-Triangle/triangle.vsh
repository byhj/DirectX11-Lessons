
struct VS_IN
{
    float4 Pos : POSITION;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
};

VS_OUT VS(VS_IN vs_in)
{	
 
   VS_OUT vs_out;
   vs_out.Pos = vs_in.Pos;
 
   return vs_out;
}
