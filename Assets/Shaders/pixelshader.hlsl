struct PixelData
{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCORRD;
};


Texture2D g_texture : register(t1);
SamplerState g_sampler : register(s0);

float4 PSMain(PixelData data ) : SV_Target 
{
    return g_texture.Sample(g_sampler,data.uv);
}