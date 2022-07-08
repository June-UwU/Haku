

struct VertexData
{
	float3 pos : POSITION;
	float4 rgba : COLOR;
};


struct PixelData
{
	float4 pos :SV_POSITION;
	float4 rgba :COLOR;
};


PixelData main(VertexData data)
{
	PixelData ret;
	ret.pos = float4(data.pos,1.0f);
	ret.rgba = data.rgba;
	return ret;	
}