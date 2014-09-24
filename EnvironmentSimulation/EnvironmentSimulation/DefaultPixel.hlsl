struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	return input.color;
}