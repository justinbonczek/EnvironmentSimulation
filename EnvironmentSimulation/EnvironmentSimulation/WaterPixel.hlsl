cbuffer terrain : register(b1)
{
	float maxHeight;
	float xOff;
	float zOff;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
};

Texture2D _Texture : register(t0);
SamplerState _Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	//return _Texture.Sample(_Sampler, float2(input.uv.x + xOff, input.uv.y + zOff));
	return float4((input.color.rgb + _Texture.Sample(_Sampler, float2(input.uv.x + xOff, input.uv.y + zOff) * 50.0).rgb) / 2.0, 0.5);
}