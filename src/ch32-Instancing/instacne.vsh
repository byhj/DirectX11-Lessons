#define NUM_LEAVES_PER_TREE 1000
#define NUM_TREE 400

cbuffer MatrixBuffer : register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};

cbuffer InstanceBuffer : register(b1)
{
	bool isTree;
	bool isLeaf;
}


cbuffer LeaveMatrixBuffer : register(b2)
{
	float4x4 leafOnTree[NUM_LEAVES_PER_TREE];
};

cbuffer TreeMatrixBuffer  :register(b3)
{
	float4 TreeModel[Num_TREE];
};


struct VS_IN
{
    float4 Pos       : POSITION;
	float3 Normal    : NORMAL;
    float2 Tex       : TEXCOORD0;
	float3 Tangent   : TANGENT;
	float3 BiTangent : BITANGENT;
};

struct VS_OUT
{
    float4 Pos       : SV_POSITION;
	float3 Normal    : NORMAL;
    float2 Tex       : TEXCOORD0;
	float3 Tangent   : TANGENT;
	float3 BiTangent : BITANGENT;
};

VS_OUT VS( VS_IN vs_in, uint instanceID : SV_InstanceID )
{	
 
   VS_OUT vs_out;
    if (isTree)
    {
   	  vs_in.Pos += TreeModel[instanceID];
    }
	if (isLeaf)
    {
        //Use different model martix for every leaves one tree
        uint currTree = (instanceID / NUM_LEAVES_PER_TREE);
    	uint currLeafInTree = instanceID - (currTree * NUM_LEAVES_PER_TREE);
    	vs_in.Pos = mul(vs_in.Pos, leafOnTree[currLeafInTree]);

		vs_in.Pos += TreeModel[currTree];
    }

   vs_out.Pos = mul(vs_in.Pos, model);
   vs_out.Pos = mul(vs_out.Pos, view);
   vs_out.Pos = mul(vs_out.Pos, proj);

   vs_out.Normal    = mul(vs_in.Normal,  (float3x3)model );
   vs_out.Tangent   = mul(vs_in.Tangent, (float3x3)model );
   vs_out.BiTangent = mul(vs_in.BiTangent, (float3x3)model );

   vs_out.Tex = vs_in.Tex;
 
   return vs_out;
}
