struct PixelData
{
	float4 p_pos : SV_POSITION;
	float4 p_col : COLOR;
};

struct VextexData
{
	float4 pos : POSITION; 
	float4 col : COLOR;
};


PixelData main(VextexData data)
{
    PixelData ret;
    ret.p_pos = data.pos;
    ret.p_col = data.col;
    return ret;
}