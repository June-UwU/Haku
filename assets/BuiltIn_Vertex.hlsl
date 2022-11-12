cbuffer global_const : register(b0)
{
	matrix proj_mat;
	matrix view_mat;
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
	matrix mat = mul(view_mat , proj_mat);
	ret.p_pos = ret.p_pos * float4(0.0f, 0.0f, 10.0f, 0.0f);
	ret.p_pos = mul(data.pos, mat);
    ret.p_col = data.col;
    return ret;
}
