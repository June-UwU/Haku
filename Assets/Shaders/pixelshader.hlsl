struct PixelData
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

float4 PSMain(PixelData data ) : SV_Target 
{
    return data.col;
}