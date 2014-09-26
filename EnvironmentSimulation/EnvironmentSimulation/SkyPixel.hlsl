struct VertexToPixel
{
	float4 posH			: SV_POSITION;
	float3 posL			: POSITION;
};

TextureCube cubeMap : register(t6);
SamplerState _Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return cubeMap.Sample(_Sampler, input.posL);
}