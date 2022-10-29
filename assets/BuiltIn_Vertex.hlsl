
cbuffer g_const : register(b0)
{
	matrix  proj;
	matrix  view;
	matrix  padd[2];	
};

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
	float4 inter = mul(data.pos, proj);
    ret.p_pos =  mul(inter , view);
    ret.p_col = data.col;
    return ret;
}