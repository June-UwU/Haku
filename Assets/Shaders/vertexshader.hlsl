
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
    ret.p_pos = float4(data.pos,1.0f);
    ret.p_col = data.col;
    return ret;
}