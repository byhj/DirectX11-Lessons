struct VS_OUTPUT 
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

//input Layout decide the in data 

VS_OUTPUT VS(float4 inPos: POSITION, float4 inColor: COLOR)
{
   VS_OUTPUT vs_out;
   vs_out.Pos = inPos;
   vs_out.Color = inColor;
 
   return vs_out;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
   return  input.Color;
}