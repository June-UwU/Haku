cbuffer transformations : register(b0)
{
	matrix transform;
	float4 pad[12];
}

cbuffer camera : register(b1)
{
	matrix camera;
	float4 padding[12];
}

struct PixelData
{
	float4 p_pos : SV_POSITION;
	float2 p_uv : TEXCORRD;
};

struct VextexData
{
	float3 pos : POSITION; 
	float2 uv : TEXCORRD;
};


PixelData VSMain(VextexData data)
{
    PixelData ret;
	matrix result = mul(transform, camera);
    ret.p_pos = mul(float4(data.pos,1.0f), 	result);
    ret.p_uv = data.uv;
    return ret;
}