cbuffer rotations : register(b0)
{
	matrix rotate;
	float4 pad[12];
}
struct PixelData
{
	float4 p_pos : SV_POSITION;
	float4 p_col : COLOR;
};

struct VextexData
{
	float3 pos : POSITION; 
	float4 col : COLOR;
};


PixelData VSMain(VextexData data)
{
    PixelData ret;
    ret.p_pos = mul(float4(data.pos,1.0f), 	rotate);
    ret.p_col = data.col;
    return ret;
}