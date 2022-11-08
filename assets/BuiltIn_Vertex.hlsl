cbuffer global_const : register(b0)
{
	matrix view_mat;
	matrix proj_mat;
	matrix padding[2];
}

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
	ret.p_pos = mul(ret.p_pos, proj_mat);
	ret.p_pos = mul(data.pos,view_mat);
    ret.p_col = data.col;
    return ret;
}
