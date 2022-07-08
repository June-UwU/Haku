

struct PixelData
{
	float4 pos :SV_POSITION;
	float4 rgba :COLOR;
};


float4 main(PixelData data) : SV_Target 
{
	return data.rgba;
}